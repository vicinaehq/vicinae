pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import Vicinae

Window {
    id: root

    readonly property bool dictating: Hud.dictation !== null
    readonly property bool dictationMessage: dictating && Hud.dictation.message !== ""
    property bool showElapsedTime: false

    property color pillColor: dictating ? Theme.background : Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.9)
    property color pillBorderColor: Config.withAlpha(Theme.divider, Config.windowOpacity)
    property int pillBorderWidth: 1
    property real pillRadius: pill.height / 2

    signal shown

    width: pill.width
    height: pill.height
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.ToolTip
    color: "transparent"
    visible: false

    onVisibleChanged: if (visible)
        shown()

    Component.onCompleted: Hud.registerWindow(root)

    component HudButton: Item {
        id: button
        property var icon
        signal clicked

        implicitWidth: 24
        implicitHeight: 24

        Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: Theme.foreground
            opacity: mouse.containsMouse ? 0.15 : 0
        }

        ViciImage {
            anchors.centerIn: parent
            width: 14
            height: 14
            source: button.icon
            sourceSize: Qt.size(14, 14)
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: button.clicked()
        }
    }

    Rectangle {
        id: pill
        width: row.width + 30
        height: row.height + 20
        radius: root.pillRadius
        color: root.pillColor
        border.color: root.pillBorderColor
        border.width: root.pillBorderWidth

        RowLayout {
            id: row
            anchors.centerIn: parent
            spacing: root.dictating ? 10 : 5

            HudButton {
                id: cancelButton
                visible: root.dictating && !root.dictationMessage
                enabled: root.dictating && Hud.dictation.showControls
                opacity: enabled ? 1 : 0.3
                icon: Img.icon(BuiltinIcon.Xmark).withFillColor(Theme.foreground)
                onClicked: Hud.dictation.cancel()
            }

            ViciImage {
                visible: !root.dictating && Hud.hasIcon
                source: Hud.icon
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                sourceSize: Qt.size(16, 16)
            }

            Text {
                visible: !root.dictating
                text: Hud.text
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                maximumLineCount: 1
                elide: Text.ElideRight
                Layout.maximumWidth: 270
            }

            Item {
                id: waveformSlot
                visible: root.dictating && !root.dictationMessage
                Layout.preferredWidth: waveform.implicitWidth
                Layout.preferredHeight: waveform.maxHeight
                Layout.alignment: Qt.AlignVCenter

                AudioWaveform {
                    id: waveform
                    anchors.centerIn: parent
                    visible: root.dictating && !Hud.dictation.transcribing
                    level: root.dictating ? Hud.dictation.audioLevel : 0
                }

                PulsingDots {
                    anchors.centerIn: parent
                    visible: root.dictating && Hud.dictation.transcribing
                    active: visible
                }
            }

            Text {
                visible: root.dictationMessage
                text: root.dictating ? Hud.dictation.message : ""
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                Layout.preferredWidth: waveform.implicitWidth + 2 * (cancelButton.implicitWidth + row.spacing)
                Layout.preferredHeight: cancelButton.implicitHeight
            }

            Text {
                visible: root.dictating && root.showElapsedTime
                text: root.dictating ? Hud.dictation.elapsedTime : ""
                color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.6)
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 44
            }

            HudButton {
                visible: root.dictating && !root.dictationMessage
                enabled: root.dictating && Hud.dictation.showControls
                opacity: enabled ? 1 : 0.3
                icon: Img.icon(BuiltinIcon.Check).withFillColor(Theme.accent)
                onClicked: Hud.dictation.accept()
            }
        }
    }
}
