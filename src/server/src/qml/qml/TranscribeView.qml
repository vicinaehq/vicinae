import QtQuick

Item {
    id: root
    required property var host

    Column {
        anchors.centerIn: parent
        spacing: 16
        width: Math.min(parent.width - 48, 280)

        AudioLevelMeter {
            width: parent.width
            height: 6
            level: root.host.audioLevel
            visible: !root.host.transcribing
        }

        PulsingDots {
            anchors.horizontalCenter: parent.horizontalCenter
            active: root.host.transcribing
            visible: root.host.transcribing
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.host.elapsedTime
            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
        }

        Text {
            visible: root.host.errorMessage.length > 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.host.errorMessage
            color: Qt.rgba(1, 0.3, 0.3, 0.9)
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallFontSize
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
