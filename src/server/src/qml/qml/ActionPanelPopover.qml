import QtQuick
import QtQuick.Controls

ViciPopover {
    id: root

    required property var controller
    property bool alignLeft: false
    property int maxHeight: 400

    focus: true
    closePolicy: Popup.CloseOnPressOutside
    modal: true
    dim: false

    readonly property int _gap: 6

    width: 400
    height: Math.min(stack.currentItem ? stack.currentItem.implicitHeight + 2 * padding : 300, root.maxHeight)

    x: root.alignLeft ? root._gap : (parent ? parent.width - width - root._gap : 0)
    y: -height - root._gap

    readonly property real _animAnchorX: root.alignLeft ? 0.0 : 1.0
    animationAnchorX: root._animAnchorX
    animationAnchorY: 0.0

    onActiveFocusChanged: {
        if (!activeFocus && root.controller.open)
            root.controller.close();
    }
    onClosed: {
        stack.clear(StackView.Immediate);
        if (root.controller.open)
            root.controller.close();
    }

    contentItem: FocusScope {
        focus: true

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

        MouseArea {
            anchors.fill: parent
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

    Connections {
        target: root.controller
        function onOpenChanged() {
            if (root.controller.open)
                root.open();
            else
                root.close();
        }
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
    }

    Connections {
        target: stack.currentItem
        ignoreUnknownSignals: true
        function onNavigateBack() {
            root.controller.pop();
        }
    }
}
