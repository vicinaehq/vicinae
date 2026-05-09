import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Popup {
    id: root

    property real bottomOffset: 0

    readonly property real _maxHeight: {
        const avail = parent ? parent.height - bottomOffset : 0;
        return avail * 0.6;
    }

    x: parent ? parent.width - width - 10 : 0
    y: parent ? parent.height - bottomOffset - height - 10 : 0
    width: 400
    height: Math.min(stack.currentItem ? stack.currentItem.implicitHeight + 2 : 300, _maxHeight)
    padding: 1
    focus: true
    closePolicy: Popup.CloseOnPressOutside
    popupType: Popup.Window
    transformOrigin: Popup.BottomRight

    onClosed: {
        stack.clear(StackView.Immediate);
        if (actionPanel.open)
            actionPanel.close();
    }
    onActiveFocusChanged: {
        if (!activeFocus && opened)
            actionPanel.close();
    }

    enter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                property: "scale"
                from: 0.95
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }

    background: Rectangle {
        radius: Config.borderRounding
        color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 0.95)
        border.color: Theme.mainWindowBorder
        border.width: 1

        layer.enabled: true
        layer.effect: MultiEffect {
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowBlur: 0.4
            shadowColor: Qt.rgba(0, 0, 0, 0.25)
            shadowVerticalOffset: 4
        }

        BackgroundEffect.enabled: Config.blurEnabled
        BackgroundEffect.radius: Config.borderRounding
    }

    contentItem: FocusScope {
        focus: true

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

        Keys.onPressed: event => {
            const nav = launcher.matchNavigationKey(event.key, event.modifiers);
            if (event.key === Qt.Key_Escape) {
                actionPanel.pop();
                event.accepted = true;
            } else if (event.key === Qt.Key_Left || event.key === Qt.Key_Backspace || nav === 3) {
                if (actionPanel.depth > 1) {
                    actionPanel.pop();
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

    Connections {
        target: stack.currentItem
        ignoreUnknownSignals: true
        function onNavigateBack() {
            actionPanel.pop();
        }
    }

    Connections {
        target: actionPanel
        function onOpenChanged() {
            if (actionPanel.open)
                root.open();
            else
                root.close();
        }
        function onPanelPushRequested(componentUrl, properties) {
            stack.push(componentUrl, properties, StackView.Immediate);
            actionPanel.onPanelPushed(stack.currentItem);
            if (typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter();
        }
        function onPanelPopRequested() {
            stack.pop(null, StackView.Immediate);
            actionPanel.onPanelPopped(stack.currentItem);
            if (stack.currentItem && typeof stack.currentItem.focusFilter === "function")
                stack.currentItem.focusFilter();
        }
        function onStackClearRequested() {
            stack.clear(StackView.Immediate);
        }
    }
}
