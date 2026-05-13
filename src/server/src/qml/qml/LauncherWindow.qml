import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects

Window {
    id: root
    property int shadowPadding: 0

    readonly property int _w: launcher.overrideWidth || Config.windowWidth
    readonly property int _h: launcher.overrideHeight || Config.windowHeight
    readonly property int _contentH: launcher.compacted ? 60 + 2 * Config.borderWidth : _h

    width: _w + 2 * shadowPadding
    height: _h + 2 * shadowPadding
    minimumWidth: _w + 2 * shadowPadding
    maximumWidth: _w + 2 * shadowPadding
    minimumHeight: _h + 2 * shadowPadding
    maximumHeight: _h + 2 * shadowPadding
    title: "Vicinae Launcher"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    BackgroundEffect.enabled: Config.blurEnabled
    BackgroundEffect.radius: Config.borderRounding
    BackgroundEffect.region: Qt.rect(shadowPadding, shadowPadding, _w, launcher.compacted ? 60 : _h)

    Item {
        id: shadowMask
        width: root.width
        height: root.height
        visible: false
        layer.enabled: true

        Rectangle {
            x: root.shadowPadding
            y: root.shadowPadding
            width: _w
            height: _contentH
            radius: Config.borderRounding
            color: "white"
        }
    }

    Item {
        id: shadowCaster
        anchors.fill: parent
        visible: root.shadowPadding > 0

        Rectangle {
            x: root.shadowPadding
            y: root.shadowPadding
            width: _w
            height: _contentH
            radius: Config.borderRounding
            color: "black"
        }

        layer.enabled: root.shadowPadding > 0
        layer.effect: MultiEffect {
            autoPaddingEnabled: false
            shadowEnabled: true
            shadowBlur: 0.5
            shadowVerticalOffset: 3
            shadowColor: Qt.rgba(0, 0, 0, 0.3)
            maskEnabled: true
            maskInverted: true
            maskSource: shadowMask
        }
    }

    Item {
        id: content
        x: root.shadowPadding
        y: root.shadowPadding
        width: _w
        height: _h

        Rectangle {
            visible: launcher.compacted
            width: _w
            height: 60 + 2 * Config.borderWidth
            radius: Config.borderRounding
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
            border.color: Theme.mainWindowBorder
            border.width: Config.borderWidth
        }

        Item {
            visible: !launcher.compacted
            anchors.fill: parent
            anchors.bottomMargin: launcher.hasOverlay || !launcher.statusBarVisible ? 0 : footer.height + Config.borderWidth
            clip: true

            Rectangle {
                width: _w
                height: _h
                radius: Config.borderRounding
                color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
            }
        }

        Item {
            visible: !launcher.compacted && !launcher.hasOverlay && launcher.statusBarVisible
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: footer.height + Config.borderWidth
            clip: true

            Rectangle {
                width: _w
                height: _h
                anchors.bottom: parent.bottom
                radius: Config.borderRounding
                color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, Config.windowOpacity)
            }
        }

        Rectangle {
            visible: !launcher.compacted
            anchors.fill: parent
            radius: Config.borderRounding
            color: "transparent"
            border.color: Theme.mainWindowBorder
            border.width: Config.borderWidth
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Config.borderWidth
            spacing: 0
            visible: !launcher.hasOverlay

            SearchBar {
                id: searchBar
                Layout.fillWidth: true
                Layout.preferredHeight: launcher.searchVisible ? 60 : 0
                visible: launcher.searchVisible
                enabled: !launcher.alertModel.visible
            }

            HorizontalLoadingBar {
                Layout.fillWidth: true
                implicitHeight: launcher.searchVisible ? 1 : 0
                visible: launcher.searchVisible && !launcher.compacted
                loading: launcher.isLoading
            }

            Item {
                id: contentArea
                objectName: "contentArea"
                Layout.fillWidth: true
                Layout.fillHeight: true

                StackView {
                    id: commandStack
                    anchors.fill: parent
                    visible: !launcher.compacted
                    initialItem: RootSearchList {}
                }
            }

            Rectangle {
                visible: !launcher.compacted && launcher.statusBarVisible
                Layout.fillWidth: true
                implicitHeight: 1
                color: Theme.divider
            }

            Footer {
                id: footer
                visible: !launcher.compacted && launcher.statusBarVisible
                Layout.fillWidth: true
                Layout.preferredHeight: visible ? 40 : 0
            }
        }

        Loader {
            id: overlayLoader
            anchors.fill: parent
            anchors.margins: Config.borderWidth
            visible: launcher.hasOverlay

            onLoaded: if (item)
                item.forceActiveFocus()
        }

        ActionPanelPopover {
            id: actionPanelPopover
            z: 100
            anchors.fill: parent
            anchors.bottomMargin: footer.height + 1 + Config.borderWidth
        }

        AlertDialog {
            id: alertDialog
            Overlay.modal: Item {
                Rectangle {
                    x: root.shadowPadding
                    y: root.shadowPadding
                    width: parent.width - 2 * root.shadowPadding
                    height: parent.height - 2 * root.shadowPadding
                    radius: root.shadowPadding > 0 ? Config.borderRounding : 0
                    color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.5)
                }
            }
        }
    }

    Connections {
        target: launcher.alertModel
        function onVisibleChanged() {
            if (launcher.alertModel.visible) {
                alertDialog.open();
            } else {
                if (alertDialog.visible)
                    alertDialog.close();
                searchBar.focusInput();
            }
        }
    }

    Connections {
        target: launcher
        function onCommandViewPushed(componentUrl, properties) {
            commandStack.push(componentUrl, properties, StackView.Immediate);
        }
        function onCommandViewReplaced(componentUrl, properties) {
            commandStack.replace(commandStack.currentItem, componentUrl, properties, StackView.Immediate);
        }
        function onCommandViewPopped() {
            if (commandStack.depth > 1)
                commandStack.pop(StackView.Immediate);
        }
        function onCommandStackCleared() {
            if (commandStack.depth > 1)
                commandStack.pop(null, StackView.Immediate);
        }
        function onOverlayChanged() {
            if (launcher.hasOverlay) {
                overlayLoader.setSource(launcher.overlayUrl, {
                    host: launcher.overlayHost
                });
            } else {
                overlayLoader.source = "";
                searchBar.focusInput();
            }
        }
    }

    Connections {
        target: Nav
        function onWindowVisiblityChanged(visible) {
            if (visible) {
                root.visible = true;
                root.raise();
                root.requestActivate();
                searchBar.focusInput();
            } else {
                root.visible = false;
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        enabled: !launcher.alertModel.visible && !actionPanel.open && !launcher.hasOverlay
        onActivated: launcher.handleEscape()
    }

    Shortcut {
        sequence: "Shift+Escape"
        enabled: !launcher.alertModel.visible
        onActivated: launcher.popToRoot()
    }

    Shortcut {
        sequence: Keybinds.toggleActionPanelSequence
        enabled: !launcher.alertModel.visible
        onActivated: {
            if (launcher.compacted)
                launcher.expand();
            actionPanel.toggle();
        }
    }

    onWidthChanged: root.x = (Screen.width - root.width) / 2
    onHeightChanged: root.y = (Screen.height - root.height) / 3

    Component.onCompleted: {
        root.x = (Screen.width - root.width) / 2;
        root.y = (Screen.height - root.height) / 3;
    }
}
