import QtQuick

BarPill {
    id: root

    readonly property string trackLabel: bar.mediaArtist !== "" ? qsTr("%1 — %2").arg(bar.mediaTitle).arg(bar.mediaArtist) : (bar.mediaTitle !== "" ? bar.mediaTitle : bar.mediaIdentity)

    visible: bar.mediaVisible
    horizontalPadding: 6
    tooltip: bar.mediaPlaying ? qsTr("Playing: %1").arg(trackLabel) : qsTr("Paused: %1").arg(trackLabel)

    onClicked: mouse => {
        if (mouse.button === Qt.MiddleButton)
            bar.mediaNext();
        else if (mouse.button === Qt.RightButton)
            bar.mediaPrevious();
        else
            bar.mediaPlayPause();
    }
    onWheel: wheel => {
        if (wheel.angleDelta.y < 0)
            bar.mediaNext();
        else if (wheel.angleDelta.y > 0)
            bar.mediaPrevious();
    }

    ViciImage {
        width: 18
        height: 18
        source: Img.builtin(bar.mediaPlaying ? "pause-filled" : "play-filled").withFillColor(bar.mediaPlaying ? Theme.foreground : Theme.textMuted)
        sourceSize: Qt.size(18, 18)
    }
}
