import QtQuick

BarPill {
    id: root

    readonly property string iconName: bar.muted || bar.volume === 0 ? "speaker-off" : (bar.volume < 34 ? "speaker-low" : "speaker-high")

    tooltip: bar.muted ? qsTr("Muted") : qsTr("Volume %1%").arg(bar.volume)
    horizontalPadding: 6

    onClicked: mouse => {
        if (mouse.button === Qt.LeftButton)
            bar.toggleMute();
    }
    onWheel: wheel => bar.adjustVolume(wheel.angleDelta.y > 0 ? 5 : -5)

    Row {
        spacing: 5

        ViciImage {
            width: 18
            height: 18
            source: Img.builtin(root.iconName).withFillColor(bar.muted ? Theme.textMuted : Theme.foreground)
            sourceSize: Qt.size(18, 18)
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("%1%").arg(bar.volume)
            color: bar.muted ? Theme.textMuted : Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize + 1
            font.weight: Font.Normal
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
