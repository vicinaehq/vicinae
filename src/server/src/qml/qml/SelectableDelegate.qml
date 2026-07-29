import QtQuick

/// Reusable delegate base for list items.  Provides a Source-blended
/// rounded-rect background that highlights on selection/hover, a MouseArea
/// for click handling, and a content slot for view-specific layouts.
Item {
    id: root

    property bool selected: false
    property bool draggable: false
    readonly property bool hovered: mouseArea.containsMouse && HoverActivation.active

    default property alias contentData: contentItem.data

    signal clicked
    signal activated
    signal dragRequested(var source)

    DraggableMouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        draggable: root.draggable
        onItemClicked: {
            root.clicked();
            if (Config.activateOnSingleClick)
                root.activated();
        }
        onItemActivated: root.activated()
        onDragRequested: root.dragRequested(root)
    }

    SourceBlendRect {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        backgroundColor: {
            var bg = Theme.background;
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
        }
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg;
                return Qt.rgba(c.r, c.g, c.b, Config.surfaceOpacity);
            }
            if (root.hovered) {
                var h = Theme.listItemHoverBg;
                return Qt.rgba(h.r, h.g, h.b, Config.surfaceOpacity);
            }
            var bg = Theme.background;
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
        }
    }

    Item {
        id: contentItem
        anchors.fill: parent
    }
}
