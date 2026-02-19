import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Item {
    id: root
    visible: launcher.actionPanelOpen || panel.opacity > 0
    enabled: launcher.actionPanelOpen
    onVisibleChanged: {
        if (!visible) {
            if (launcher.actionPanelModel)
                launcher.actionPanelModel.setFilter("")
            stack.clear(StackView.Immediate)
        }
    }

    // Expose StackView so ActionListPanel can push submenus
    property alias stackView: stack

    function popSubmenu() {
        if (stack.depth > 1) {
            stack.pop(null, StackView.Immediate)
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter()
        } else {
            launcher.closeActionPanel()
        }
    }

    // Backdrop for click-away dismissal
    MouseArea {
        anchors.fill: parent
        onClicked: launcher.closeActionPanel()
    }

    // Panel container
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
        color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g,
                       Theme.statusBarBackground.b, Config.windowOpacity)
        border.color: Theme.mainWindowBorder
        border.width: Config.borderWidth

        opacity: launcher.actionPanelOpen ? 1 : 0
        scale: launcher.actionPanelOpen ? 1.0 : 0.95
        transformOrigin: Item.BottomRight

        Behavior on opacity {
            NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
        }
        Behavior on scale {
            NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
        }

        layer.enabled: true
        layer.effect: MultiEffect {
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowBlur: 0.4
            shadowColor: Qt.rgba(0, 0, 0, 0.25)
            shadowVerticalOffset: 4
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
                pushEnter: null
                pushExit: null
                popEnter: null
                popExit: null
                replaceEnter: null
                replaceExit: null

                initialItem: ActionListPanel {
                    model: launcher.actionPanelModel
                }
            }
        }
    }

    // Navigate back from submenu via ActionListPanel signal
    Connections {
        target: stack.currentItem
        ignoreUnknownSignals: true
        function onNavigateBack() { root.popSubmenu() }
    }

    // Keyboard handling
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape) {
            root.popSubmenu()
            event.accepted = true
        } else if (event.key === Qt.Key_Left || event.key === Qt.Key_Backspace) {
            if (stack.depth > 1) {
                root.popSubmenu()
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

    Connections {
        target: launcher
        function onActionPanelOpenChanged() {
            if (launcher.actionPanelOpen) {
                // Always rebuild from current model on open
                stack.clear(StackView.Immediate)
                if (launcher.actionPanelModel)
                    stack.push(rootPanelComponent, { "model": launcher.actionPanelModel },
                               StackView.Immediate)
                if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                    stack.currentItem.focusFilter()
            }
        }
        function onActionPanelModelChanged() {
            if (!launcher.actionPanelOpen) return
            stack.clear(StackView.Immediate)
            if (launcher.actionPanelModel) {
                stack.push(rootPanelComponent, { "model": launcher.actionPanelModel },
                           StackView.Immediate)
            }
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter()
        }
        function onActionPanelSubmenuPushed(subModel) {
            stack.push(rootPanelComponent, { "model": subModel }, StackView.Immediate)
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter()
        }
    }

    Component {
        id: rootPanelComponent
        ActionListPanel {}
    }
}
