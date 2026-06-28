import QtQuick
import QtQuick.Controls

Popup {
    id: root

    required property var controller
    property bool alignLeft: false
    property int maxHeight: 400

    popupType: Popup.Window
    focus: true
    padding: 1
    closePolicy: Popup.CloseOnPressOutside

    width: 400
    height: Math.min(stack.currentItem ? stack.currentItem.implicitHeight + 2 * padding : 300, root.maxHeight)

    x: root.alignLeft ? 0 : (parent ? parent.width - width : 0)
    y: -height - 6
    PopupPlacement.alignment: Qt.AlignTop | (root.alignLeft ? Qt.AlignLeft : Qt.AlignRight)

    readonly property bool _nativeAnim: Qt.platform.os === "osx" && popupMaterial.macImpl !== null
    readonly property real _animAnchorX: root.alignLeft ? 0.0 : 1.0

    onAboutToShow: if (popupMaterial.macImpl)
        popupMaterial.macImpl.animateIn(root._animAnchorX, 0.0)
    onAboutToHide: if (popupMaterial.macImpl)
        popupMaterial.macImpl.animateOut(root._animAnchorX, 0.0)

    onActiveFocusChanged: {
        if (!activeFocus && root.controller.open)
            root.controller.close();
    }
    onClosed: {
        stack.clear(StackView.Immediate);
        if (root.controller.open)
            root.controller.close();
    }

    enter: root._nativeAnim ? null : _itemEnter
    exit: root._nativeAnim ? _holdExit : _itemExit

    property Transition _itemEnter: Transition {
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

    property Transition _itemExit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 100
                easing.type: Easing.InCubic
            }
            NumberAnimation {
                property: "scale"
                from: 1
                to: 0.95
                duration: 100
                easing.type: Easing.InCubic
            }
        }
    }

    property Transition _holdExit: Transition {
        PauseAnimation {
            duration: 110
        }
    }

    background: Rectangle {
        radius: Math.min(Config.borderRounding, 15)
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, Config.windowOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, Config.windowOpacity)
        border.width: 1

        PopupMaterial {
            id: popupMaterial
        }
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
        function onStackClearRequested() {
            stack.clear(StackView.Immediate);
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
