import QtQuick
import QtQuick.Controls

ToolTip {
    id: root
    delay: 500

    contentItem: Text {
        text: root.text
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
    }

    background: Rectangle {
        color: Theme.secondaryBackground
        border.color: Theme.divider
        border.width: 1
        radius: 4
    }
}
