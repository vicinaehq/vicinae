import QtQuick

/// Reusable delegate base for list items.  Provides a Source-blended
/// rounded-rect background that highlights on selection/hover, a MouseArea
/// for click handling, and a content slot for view-specific layouts.
Item {
    id: root

    property bool selected: false
    readonly property bool hovered: mouseArea.containsMouse

    default property alias contentData: contentItem.data

    signal clicked()
    signal doubleClicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        onDoubleClicked: root.doubleClicked()
    }

    SourceBlendRect {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        backgroundColor: {
            var bg = Theme.background
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
        }
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg
                return Qt.rgba(c.r, c.g, c.b, Config.windowOpacity)
            }
            if (root.hovered) {
                var h = Theme.listItemHoverBg
                return Qt.rgba(h.r, h.g, h.b, Config.windowOpacity)
            }
            var bg = Theme.background
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity)
        }
    }

    Item {
        id: contentItem
        anchors.fill: parent
    }
}
