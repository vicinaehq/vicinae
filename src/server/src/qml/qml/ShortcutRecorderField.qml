import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: recorder

    property var validateShortcut: null
    property var shortcutKeysProvider: null

    signal shortcutCaptured(int key, int modifiers)

    width: 250
    height: 80
    focus: true
    closePolicy: Popup.CloseOnPressOutside
    padding: 10

    property var _currentKeys: []
    property string _statusText: "Recording..."
    property color _statusColor: Theme.foreground

    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: recorder.close()
    }

    function show(targetItem) {
        _currentKeys = []
        _statusText = "Recording..."
        _statusColor = Theme.foreground
        closeTimer.stop()

        var pos = targetItem.mapToItem(recorder.parent, 0, 0)
        recorder.x = pos.x + targetItem.width / 2 - recorder.width / 2
        recorder.y = pos.y - recorder.height - 10
        recorder.open()
    }

    onOpened: keyReceiver.forceActiveFocus()
    onActiveFocusChanged: if (!activeFocus && opened) close()

    background: Rectangle {
        radius: 8
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g,
                       Theme.secondaryBackground.b, Config.windowOpacity)
        border.color: Theme.divider
        border.width: 1
    }

    contentItem: FocusScope {
        focus: true

        Keys.onPressed: (event) => {
            event.accepted = true
            closeTimer.stop()

            var key = event.key
            var mods = event.modifiers

            var isModKey = key === Qt.Key_Shift || key === Qt.Key_Control
                           || key === Qt.Key_Alt || key === Qt.Key_Meta
            var isCloseKey = key === Qt.Key_Escape || key === Qt.Key_Backspace

            if (!isModKey && isCloseKey && mods === Qt.NoModifier) {
                recorder.close()
                return
            }

            if (recorder.shortcutKeysProvider)
                recorder._currentKeys = recorder.shortcutKeysProvider(key, mods)

            if (isModKey) {
                recorder._statusText = "Recording..."
                recorder._statusColor = Theme.foreground
                return
            }

            if (recorder.validateShortcut) {
                var error = recorder.validateShortcut(key, mods)
                if (error !== "") {
                    recorder._statusText = error
                    recorder._statusColor = Theme.danger
                    return
                }
            }

            recorder._statusText = "Keybind updated"
            recorder._statusColor = Theme.toastSuccess
            closeTimer.start()
            recorder.shortcutCaptured(key, mods)
        }

        Item {
            id: keyReceiver
            focus: true
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 5

            Row {
                Layout.alignment: Qt.AlignHCenter
                spacing: 5
                visible: recorder._currentKeys.length > 0

                Repeater {
                    model: recorder._currentKeys

                    delegate: Rectangle {
                        width: Math.max(25, badgeContent.implicitWidth + 10)
                        height: 25
                        radius: 6
                        color: "transparent"
                        border.color: Theme.listItemSelectionBg
                        border.width: 1

                        Loader {
                            id: badgeContent
                            anchors.centerIn: parent

                            sourceComponent: modelData.type === "icon" ? iconBadge : textBadge

                            Component {
                                id: iconBadge
                                ViciImage {
                                    source: Img.builtin(modelData.value)
                                        .withFillColor(recorder._statusColor)
                                    width: 15; height: 15
                                    sourceSize.width: 15; sourceSize.height: 15
                                }
                            }

                            Component {
                                id: textBadge
                                Text {
                                    text: modelData.value
                                    color: recorder._statusColor
                                    font.pointSize: Theme.smallerFontSize
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                text: recorder._statusText
                color: recorder._statusColor
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
            }
        }
    }
}
