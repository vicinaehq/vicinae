pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import Vicinae

Window {
    id: root

    readonly property bool dictating: Hud.dictation !== null
    readonly property bool dictationMessage: dictating && Hud.dictation.message !== ""
    readonly property bool working: dictating && !dictationMessage && (Hud.dictation.transcribing || Hud.dictation.rewriting)
    readonly property bool recording: dictating && !dictationMessage && !working
    readonly property bool captureReady: recording && Hud.dictation.captureReady
    readonly property color mutedColor: Config.withAlpha(Theme.foreground, 0.6)
    property bool showElapsedTime: false

    property color pillColor: Config.withAlpha(Theme.background, dictating ? 0.98 : 0.9)
    property color pillBorderColor: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.12 : 0.18)
    property int pillBorderWidth: 1
    property real pillRadius: height / 2

    signal shown

    width: dictating ? (dictationMessage ? Math.max(216, Math.min(360, messageLabel.implicitWidth + 72)) : working ? Math.max(216, Math.min(360, workLabel.implicitWidth + 88)) : 172) + (showElapsedTime && recording ? 48 : 0) : noticeRow.width + 30
    height: dictating ? 42 : noticeRow.height + 20
    title: dictating ? qsTr("Dictation") : Hud.text
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.ToolTip | Qt.WindowDoesNotAcceptFocus
    color: "transparent"
    visible: false

    Behavior on width {
        enabled: root.visible && root.dictating
        NumberAnimation {
            duration: 320
            easing.type: Easing.OutCubic
        }
    }

    onVisibleChanged: {
        if (visible) {
            appear.restart();
            shown();
        }
    }

    NumberAnimation {
        id: appear
        target: pill
        property: "opacity"
        from: 0
        to: 1
        duration: 180
        easing.type: Easing.OutQuad
    }

    Component.onCompleted: Hud.registerWindow(root)

    component HudButton: Item {
        id: button
        property var icon
        required property string label
        property bool status: false
        readonly property bool hovered: mouse.containsMouse
        signal clicked

        implicitWidth: 28
        implicitHeight: 28
        Accessible.role: Accessible.Button
        Accessible.name: label
        Accessible.onPressAction: if (enabled)
            clicked()

        Rectangle {
            anchors.centerIn: parent
            width: 22
            height: 22
            radius: width / 2
            color: Config.withAlpha(button.hovered ? Theme.accent : Theme.foreground, button.hovered ? 0.18 : 0.08)
            opacity: button.status && !button.hovered ? 0 : 1
            scale: mouse.pressed ? 0.92 : button.hovered ? 1.05 : 1

            Behavior on color {
                ColorAnimation {
                    duration: 150
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
            Behavior on scale {
                NumberAnimation {
                    duration: 100
                }
            }
        }

        ViciImage {
            anchors.centerIn: parent
            visible: !button.status
            width: 11
            height: 11
            source: button.icon
            sourceSize: Qt.size(11, 11)
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: button.clicked()
        }

        ViciToolTip {
            text: button.label
            visible: button.hovered
        }
    }

    Rectangle {
        id: pill
        anchors.fill: parent
        radius: root.pillRadius
        color: root.pillColor
        border.color: root.pillBorderColor
        border.width: root.pillBorderWidth

        RowLayout {
            id: noticeRow
            visible: !root.dictating
            anchors.centerIn: parent
            spacing: 5

            ViciImage {
                visible: Hud.hasIcon
                source: Hud.icon
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                sourceSize: Qt.size(16, 16)
            }

            Text {
                text: Hud.text
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                maximumLineCount: 1
                elide: Text.ElideRight
                Layout.maximumWidth: 270
            }
        }

        Item {
            anchors.fill: parent
            visible: root.dictating

            HudButton {
                id: finishButton
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                visible: root.recording
                enabled: root.dictating && Hud.dictation.showControls
                label: qsTr("Stop and transcribe")
                status: true
                onClicked: if (Hud.dictation)
                    Hud.dictation.accept()

                Rectangle {
                    anchors.centerIn: parent
                    width: 7
                    height: 7
                    radius: finishButton.hovered ? 2 : width / 2
                    color: root.captureReady ? Theme.accent : root.mutedColor
                    opacity: root.captureReady ? 1 : 0.45
                    Behavior on radius {
                        NumberAnimation {
                            duration: 120
                        }
                    }

                    Rectangle {
                        id: pulseHalo
                        anchors.centerIn: parent
                        width: 7
                        height: 7
                        radius: width / 2
                        color: "transparent"
                        border.color: Theme.accent
                        border.width: 1
                        visible: root.captureReady && !finishButton.hovered

                        ParallelAnimation {
                            running: root.visible && root.captureReady && !finishButton.hovered
                            loops: Animation.Infinite
                            NumberAnimation {
                                target: pulseHalo
                                property: "scale"
                                from: 1
                                to: 2.8
                                duration: 1900
                                easing.type: Easing.OutQuad
                            }
                            NumberAnimation {
                                target: pulseHalo
                                property: "opacity"
                                from: 0.35
                                to: 0
                                duration: 1900
                                easing.type: Easing.OutQuad
                            }
                        }
                    }
                }
            }

            ViciSpinner {
                anchors.left: parent.left
                anchors.leftMargin: 14
                anchors.verticalCenter: parent.verticalCenter
                visible: root.working
                width: 13
                height: 13
                color: Theme.accent
            }

            AudioWaveform {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: root.showElapsedTime ? -24 : 0
                visible: root.recording
                active: root.visible && root.recording
                ready: root.captureReady
                level: root.captureReady ? Hud.dictation.audioLevel : 0
            }

            Text {
                id: workLabel
                anchors.centerIn: parent
                width: parent.width - 80
                visible: root.working
                text: root.dictating && Hud.dictation.rewriting ? qsTr("Rewriting") : qsTr("Transcribing")
                color: root.mutedColor
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                Accessible.role: Accessible.StaticText
                Accessible.name: text
            }

            Text {
                id: messageLabel
                anchors.left: parent.left
                anchors.leftMargin: 18
                anchors.right: cancelButton.left
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                visible: root.dictationMessage
                text: root.dictating ? Hud.dictation.message : ""
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                Accessible.role: Accessible.StaticText
                Accessible.name: text
            }

            Text {
                anchors.right: cancelButton.left
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                visible: root.recording && root.showElapsedTime
                text: root.dictating ? Hud.dictation.elapsedTime : ""
                color: root.mutedColor
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
            }

            HudButton {
                id: cancelButton
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                label: root.dictationMessage ? qsTr("Dismiss") : qsTr("Cancel dictation")
                icon: Img.icon(BuiltinIcon.Xmark).withFillColor(hovered ? Theme.foreground : root.mutedColor)
                onClicked: if (Hud.dictation)
                    Hud.dictation.cancel()
            }
        }
    }
}
