import QtQuick
import QtQuick.Controls
import QtQuick.Effects

FocusRestoringScope {
    id: root

    required property var controller
    property bool alignLeft: false

    active: root.controller.open
    visible: root.controller.open || panel.opacity > 0
    enabled: root.controller.open

    onVisibleChanged: {
        if (!visible)
            stack.clear(StackView.Immediate);
    }
    onActiveFocusChanged: {
        if (!activeFocus && root.controller.open)
            root.controller.close();
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.controller.close()
    }

    Item {
        id: panel
        x: root.alignLeft ? 10 : parent.width - width - 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        width: 400
        height: Math.min(stack.currentItem ? stack.currentItem.implicitHeight + 2 : 300, root.height * 0.6)

        opacity: 0
        scale: 0.95
        transformOrigin: root.alignLeft ? Item.BottomLeft : Item.BottomRight

        states: State {
            name: "open"
            when: root.controller.open
            PropertyChanges {
                target: panel
                opacity: 1
                scale: 1.0
            }
        }
        transitions: Transition {
            to: "open"
            NumberAnimation {
                properties: "opacity,scale"
                duration: 150
                easing.type: Easing.OutCubic
            }
        }

        Rectangle {
            id: panelShadow
            anchors.fill: parent
            radius: Config.borderRounding
            color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 1)

            layer.enabled: true
            layer.effect: MultiEffect {
                autoPaddingEnabled: true
                shadowEnabled: true
                shadowBlur: 0.4
                shadowColor: Qt.rgba(0, 0, 0, 0.25)
                shadowVerticalOffset: 4
            }
        }

        Rectangle {
            id: panelContent
            anchors.fill: parent
            radius: Config.borderRounding
            color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 1)
            border.color: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            border.width: 1

            MouseArea {
                anchors.fill: parent
                anchors.margins: 1
                acceptedButtons: Qt.NoButton
                onWheel: function (wheel) {
                    wheel.accepted = true;
                }

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
    }

    Connections {
        target: stack.currentItem
        ignoreUnknownSignals: true
        function onNavigateBack() {
            root.controller.pop();
        }
    }

    Connections {
        target: root.controller
        function onPanelPushRequested(componentUrl, properties) {
            stack.push(componentUrl, properties, StackView.Immediate);
            stack.currentItem.controller = root.controller;
            root.controller.onPanelPushed(stack.currentItem);
            if (typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter();
        }
        function onPanelPopRequested() {
            stack.pop();
            root.controller.onPanelPopped(stack.currentItem);
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter();
        }
        function onStackClearRequested() {
            stack.clear(StackView.Immediate);
        }
    }

    Keys.onPressed: event => {
        const nav = launcher.matchNavigationKey(event.key, event.modifiers);
        if (event.key === Qt.Key_Escape) {
            root.controller.pop();
            event.accepted = true;
        } else if (event.key === Qt.Key_Left || nav === 3) {
            if (root.controller.depth > 1) {
                root.controller.pop();
                event.accepted = true;
            }
        } else if (event.key === Qt.Key_Up || nav === 1) {
            if (stack.currentItem) {
                const ctrl = (event.modifiers & Qt.ControlModifier);
                if (ctrl && typeof stack.currentItem.moveSectionUp === "function")
                    stack.currentItem.moveSectionUp();
                else if (typeof stack.currentItem.moveUp === "function")
                    stack.currentItem.moveUp();
            }
            event.accepted = true;
        } else if (event.key === Qt.Key_Down || nav === 2) {
            if (stack.currentItem) {
                const ctrl = (event.modifiers & Qt.ControlModifier);
                if (ctrl && typeof stack.currentItem.moveSectionDown === "function")
                    stack.currentItem.moveSectionDown();
                else if (typeof stack.currentItem.moveDown === "function")
                    stack.currentItem.moveDown();
            }
            event.accepted = true;
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            if (stack.currentItem && typeof stack.currentItem.activateCurrent === "function")
                stack.currentItem.activateCurrent();
            event.accepted = true;
        }
    }
}
