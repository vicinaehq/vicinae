import QtQuick
import QtQuick.Controls
import Vicinae

Rectangle {
    id: root

    property var iconSource
    property bool danger: false
    property string tooltip: ""
    property real iconSize: 14

    signal clicked

    implicitWidth: 26
    implicitHeight: 26
    radius: 6
    color: hover.hovered ? (root.danger ? Config.withAlpha(Theme.danger, 0.15) : Theme.listItemHoverBg) : "transparent"

    ViciImage {
        anchors.centerIn: parent
        width: root.iconSize
        height: root.iconSize
        source: root.iconSource.withFillColor(hover.hovered && root.danger ? Theme.danger : Theme.textMuted)
    }

    HoverHandler {
        id: hover
        cursorShape: Qt.PointingHandCursor
    }

    TapHandler {
        onTapped: root.clicked()
    }

    ToolTip.visible: root.tooltip !== "" && hover.hovered
    ToolTip.text: root.tooltip
    ToolTip.delay: 600
}
