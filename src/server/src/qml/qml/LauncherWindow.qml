import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects

Window {
    id: root
    property int shadowPadding: 0

    property int cornerRadius: Config.borderRounding
    property bool blurEnabled: Config.blurEnabled
    property bool shadowEnabled: shadowPadding > 0
    property bool nativeChrome: false

    readonly property int _w: launcher.overrideWidth || Config.windowWidth
    readonly property int _h: launcher.overrideHeight || Config.windowHeight
    // When compacted, the visible frame collapses to "search bar + (optional toolbar)".
    // The toolbar (Footer) stays visible whenever the status bar is, even when compacted —
    // so the frame must reserve space for it: 60 search + 1 divider + 40 footer.
    readonly property int _h_compact: 60 + (launcher.statusBarVisible ? (1 + 40) : 0) + 2 * Config.borderWidth
    readonly property int _contentH: launcher.compacted ? _h_compact : _h

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

    BackgroundEffect.enabled: root.blurEnabled && !root.nativeChrome
    BackgroundEffect.radius: root.cornerRadius
    BackgroundEffect.region: Qt.rect(shadowPadding, shadowPadding, _w, launcher.compacted ? (_h_compact - 2 * Config.borderWidth) : _h)

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
        visible: root.shadowEnabled && !root.nativeChrome

        Rectangle {
            x: root.shadowPadding
            y: root.shadowPadding
            width: _w
            height: _contentH
            radius: root.cornerRadius
            color: "black"
        }

        layer.enabled: root.shadowEnabled && !root.nativeChrome
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
            height: _h_compact
            radius: root.cornerRadius
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        }

        SourceBlendRect {
            visible: launcher.compacted && !root.nativeChrome
            width: _w
            height: _h_compact
            radius: root.cornerRadius
            overlay: true
            borderColor: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            borderWidth: Config.borderWidth
        }

        Item {
            visible: !launcher.compacted
            anchors.fill: parent
            anchors.bottomMargin: launcher.hasOverlay || !launcher.statusBarVisible ? 0 : footer.height + Config.borderWidth
            clip: true

            Rectangle {
                width: _w
                height: _h
                radius: root.cornerRadius
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
                radius: root.cornerRadius
                color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, Config.windowOpacity)
            }
        }

        SourceBlendRect {
            visible: !launcher.compacted && !root.nativeChrome
            anchors.fill: parent
            radius: root.cornerRadius
            overlay: true
            borderColor: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            borderWidth: Config.borderWidth
        }

        ColumnLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: Config.borderWidth
            anchors.leftMargin: Config.borderWidth
            anchors.rightMargin: Config.borderWidth
            height: _contentH - 2 * Config.borderWidth
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
                visible: !launcher.compacted
                Layout.fillWidth: true
                Layout.fillHeight: !launcher.compacted

                StackView {
                    id: commandStack
                    anchors.fill: parent
                    visible: !launcher.compacted
                }
            }

            ViciDivider {
                visible: launcher.statusBarVisible
                Layout.fillWidth: true
            }

            Footer {
                id: footer
                visible: launcher.statusBarVisible
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
            controller: actionPanel
            anchors.fill: parent
            anchors.bottomMargin: footer.height + 1 + Config.borderWidth
        }

        ActionPanelPopover {
            id: footerMenuPopover
            z: 100
            controller: footerPanel
            alignLeft: true
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

    function _centerOnCursorScreen() {
        if (!launcher.canPositionWindow)
            return;
        const g = launcher.cursorScreenGeometry();
        root.x = g.x + (g.width - root.width) / 2;
        root.y = g.y + (g.height - root.height) / 3;
    }

    Connections {
        target: Nav
        function onWindowVisiblityChanged(visible) {
            if (visible) {
                root._centerOnCursorScreen();
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
        enabled: !launcher.alertModel.visible && !actionPanel.open && !footerPanel.open && !launcher.hasOverlay
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

    onWidthChanged: {
        if (launcher.canPositionWindow)
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
    }
    onHeightChanged: {
        if (launcher.canPositionWindow)
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
    }

    Component.onCompleted: {
        if (launcher.canPositionWindow) {
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
        }
        _centerOnCursorScreen();
    }
}
