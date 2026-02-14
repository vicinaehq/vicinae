import QtQuick
import QtQuick.Controls

Item {
    id: root
    visible: launcher.actionPanelOpen

    // Expose StackView so ActionListPanel can push submenus
    property alias stackView: stack

    // Backdrop for click-away dismissal
    MouseArea {
        anchors.fill: parent
        onClicked: launcher.closeActionPanel()
    }

    // Panel container — semi-transparent overlay showing window content through
    Rectangle {
        id: panel
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        width: 400
        height: Math.min(stack.currentItem ? stack.currentItem.implicitHeight + 2 * Config.borderWidth : 300,
                         root.height * 0.6)
        radius: Config.borderRounding
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b,
                       Config.windowOpacity)
        border.color: Theme.mainWindowBorder
        border.width: Config.borderWidth
        clip: true

        // Enter/exit animations
        opacity: root.visible ? 1 : 0
        transform: Translate { y: root.visible ? 0 : 8 }

        Behavior on opacity {
            NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
        }

        // Block wheel events from propagating to the main list
        MouseArea {
            anchors.fill: parent
            anchors.margins: Config.borderWidth
            acceptedButtons: Qt.NoButton
            onWheel: function(wheel) { wheel.accepted = true }

            StackView {
                id: stack
                anchors.fill: parent
                clip: true

                initialItem: ActionListPanel {
                    model: launcher.actionPanelModel
                }
            }
        }
    }

    // Keyboard handling
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape) {
            if (stack.depth > 1) {
                stack.pop()
            } else {
                launcher.closeActionPanel()
            }
            event.accepted = true
        } else if (event.key === Qt.Key_Left) {
            if (stack.depth > 1) {
                stack.pop()
                event.accepted = true
            }
        } else if (event.key === Qt.Key_Up) {
            if (stack.currentItem && typeof stack.currentItem.moveUp === "function")
                stack.currentItem.moveUp()
            event.accepted = true
        } else if (event.key === Qt.Key_Down) {
            if (stack.currentItem && typeof stack.currentItem.moveDown === "function")
                stack.currentItem.moveDown()
            event.accepted = true
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            if (stack.currentItem && typeof stack.currentItem.activateCurrent === "function")
                stack.currentItem.activateCurrent()
            event.accepted = true
        }
    }

    // Reset stack when panel opens or model changes
    Connections {
        target: launcher
        function onActionPanelOpenChanged() {
            if (launcher.actionPanelOpen) {
                // Reset stack and focus filter
                while (stack.depth > 1) stack.pop(null, StackView.Immediate)
                if (stack.currentItem && typeof stack.currentItem.focusFilter === "function") {
                    stack.currentItem.focusFilter()
                }
            }
        }
        function onActionPanelModelChanged() {
            // Replace the root item when model changes
            stack.clear(StackView.Immediate)
            if (launcher.actionPanelModel) {
                stack.push(rootPanelComponent, { "model": launcher.actionPanelModel },
                           StackView.Immediate)
            }
        }
        function onActionPanelSubmenuPushed(subModel) {
            stack.push(rootPanelComponent, { "model": subModel })
        }
    }

    Component {
        id: rootPanelComponent
        ActionListPanel {}
    }
}
