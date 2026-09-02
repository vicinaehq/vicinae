pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects

/// Reusable delegate base for list items.  Provides a Source-blended
/// rounded-rect background that highlights on selection/hover, a MouseArea
/// for click handling, and a content slot for view-specific layouts.
Item {
    id: root

    property bool selected: false
    property bool draggable: false
    property int quickAccessIndex: -1
    readonly property bool hovered: mouseArea.containsMouse && HoverActivation.active
    readonly property bool quickAccessActive: launcher.commandHeld && quickAccessIndex >= 0 && !launcher.alertModel.visible && !actionPanel.open && !footerPanel.open && !launcher.hasOverlay
    property real quickAccessProgress: quickAccessActive ? 1 : 0

    Behavior on quickAccessProgress {
        NumberAnimation {
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

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
        layer.enabled: root.quickAccessProgress > 0
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: quickAccessMask
        }
    }

    Rectangle {
        id: quickAccessMask
        visible: false
        width: contentItem.width
        height: contentItem.height
        layer.enabled: true
        layer.smooth: true
        gradient: Gradient {
            orientation: Gradient.Horizontal

            GradientStop {
                position: Math.max(0, 1 - (quickAccessBadge.width + 28) / Math.max(1, quickAccessMask.width))
                color: "white"
            }
            GradientStop {
                position: Math.max(0, 1 - (quickAccessBadge.width + 12) / Math.max(1, quickAccessMask.width))
                color: Qt.rgba(1, 1, 1, 1 - root.quickAccessProgress)
            }
            GradientStop {
                position: 1
                color: Qt.rgba(1, 1, 1, 1 - root.quickAccessProgress)
            }
        }
    }

    ShortcutBadge {
        id: quickAccessBadge
        visible: opacity > 0
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        z: 2
        tokens: [
            {
                text: root.quickAccessIndex === 9 ? "0" : String(root.quickAccessIndex + 1)
            }
        ]
        opacity: root.quickAccessActive ? 1 : 0
        transform: Translate {
            x: root.quickAccessActive ? 0 : 8

            Behavior on x {
                NumberAnimation {
                    duration: 120
                    easing.type: Easing.OutCubic
                }
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 120
                easing.type: Easing.OutCubic
            }
        }
    }
}
