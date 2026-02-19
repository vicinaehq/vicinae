import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Item {
    id: root
    visible: actionPanel.open || panel.opacity > 0
    enabled: actionPanel.open
    onVisibleChanged: {
        if (!visible)
            stack.clear(StackView.Immediate)
    }

    // Backdrop for click-away dismissal
    MouseArea {
        anchors.fill: parent
        onClicked: actionPanel.close()
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

        opacity: actionPanel.open ? 1 : 0
        scale: actionPanel.open ? 1.0 : 0.95
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
            }
        }
    }

    // Navigate back from submenu via ActionListPanel signal
    Connections {
        target: stack.currentItem
        ignoreUnknownSignals: true
        function onNavigateBack() {
            actionPanel.pop()
        }
    }

    // Controller-driven stack management
    Connections {
        target: actionPanel
        function onPanelPushRequested(componentUrl, properties) {
            stack.push(componentUrl, properties, StackView.Immediate)
            actionPanel.onPanelPushed(stack.currentItem)
            if (typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter()
        }
        function onPanelPopRequested() {
            stack.pop(null, StackView.Immediate)
            actionPanel.onPanelPopped(stack.currentItem)
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter()
        }
        function onStackClearRequested() {
            stack.clear(StackView.Immediate)
        }
    }

    // Keyboard handling
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape) {
            actionPanel.pop()
            event.accepted = true
        } else if (event.key === Qt.Key_Left || event.key === Qt.Key_Backspace) {
            if (actionPanel.depth > 1) {
                actionPanel.pop()
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
}
