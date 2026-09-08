pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Shapes
import Vicinae

Window {
    id: root
    property int shadowPadding: 0

    property int cornerRadius: Config.borderRounding
    property bool blurEnabled: Config.blurEnabled
    property bool shadowEnabled: shadowPadding > 0
    property bool nativeChrome: false
    property bool autoPlaceOnShow: true
    readonly property int statusBarOverlap: floatingStatusBar.visible && Config.floatingStatusBar ? floatingStatusBar.height - Config.borderWidth : 0
    readonly property real statusBarTop: shadowPadding + floatingStatusBar.y
    readonly property Item popupBackdrop: contentArea
    signal aboutToShow
    signal shown

    readonly property int _w: Launcher.overrideWidth || Config.windowWidth
    readonly property int _h: Launcher.overrideHeight || Config.windowHeight
    readonly property int _contentH: Launcher.compacted ? 60 + 2 * Config.borderWidth : root._h

    width: root._w + 2 * shadowPadding
    height: root._h + 2 * shadowPadding
    minimumWidth: root._w + 2 * shadowPadding
    maximumWidth: root._w + 2 * shadowPadding
    minimumHeight: root._h + 2 * shadowPadding
    maximumHeight: root._h + 2 * shadowPadding
    title: qsTr("Vicinae Launcher")
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    WindowMaterial.enabled: root.blurEnabled && !root.nativeChrome
    WindowMaterial.radius: root.cornerRadius
    WindowMaterial.region: Qt.rect(shadowPadding, shadowPadding, root._w, Launcher.compacted ? root._contentH : root._h)

    Item {
        id: shadowMask
        width: root.width
        height: root.height
        visible: false
        layer.enabled: true

        Rectangle {
            x: root.shadowPadding
            y: root.shadowPadding
            width: root._w
            height: root._contentH
            radius: Config.borderRounding
            color: "white"
        }
    }

    Item {
        id: shadowCaster
        anchors.fill: parent
        visible: root.shadowEnabled && !root.nativeChrome

        RectangularShadow {
            x: root.shadowPadding
            y: root.shadowPadding
            width: root._w
            height: root._contentH
            radius: root.cornerRadius
            blur: root.shadowPadding
            color: Qt.rgba(0, 0, 0, 0.3)
        }

        layer.enabled: root.shadowEnabled && !root.nativeChrome
        layer.effect: MultiEffect {
            maskEnabled: true
            maskInverted: true
            maskSource: shadowMask
        }
    }

    Item {
        id: content
        x: root.shadowPadding
        y: root.shadowPadding
        width: root._w
        height: root._h

        Rectangle {
            visible: Launcher.compacted
            width: root._w
            height: 60 + 2 * Config.borderWidth
            radius: root.cornerRadius
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        }

        SourceBlendRect {
            visible: Launcher.compacted && !root.nativeChrome
            width: root._w
            height: 60 + 2 * Config.borderWidth
            radius: root.cornerRadius
            overlay: true
            borderColor: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            borderWidth: Config.borderWidth
        }

        Rectangle {
            visible: !Launcher.compacted
            width: root._w
            height: root._h
            radius: root.cornerRadius
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Config.borderWidth
            spacing: 0
            visible: !Launcher.hasOverlay

            SearchBar {
                id: searchBar
                commandStack: commandStack
                Layout.fillWidth: true
                Layout.preferredHeight: Launcher.searchVisible ? 60 : 0
                visible: Launcher.searchVisible
                enabled: !Launcher.alertModel.visible
            }

            HorizontalLoadingBar {
                Layout.fillWidth: true
                implicitHeight: Launcher.searchVisible ? 1 : 0
                visible: Launcher.searchVisible && !Launcher.compacted
                loading: Launcher.isLoading
            }

            Item {
                id: contentViewport
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    anchors.fill: parent
                    anchors.bottomMargin: floatingStatusBar.visible ? floatingStatusBar.height - Config.borderWidth : 0
                    clip: true

                    Item {
                        id: contentArea
                        objectName: "contentArea"
                        width: contentViewport.width
                        height: Config.floatingStatusBar ? contentViewport.height : parent.height

                        StackView {
                            id: commandStack
                            anchors.fill: parent
                            visible: !Launcher.compacted
                        }
                    }
                }
            }
        }

        Item {
            id: floatingStatusBar
            visible: !Launcher.compacted && !Launcher.hasOverlay && Launcher.statusBarVisible
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 41 + Config.borderWidth
            clip: true

            readonly property int backdropPad: 64

            ShaderEffectSource {
                id: statusBarBackdrop
                visible: false
                sourceItem: Config.floatingStatusBar ? contentArea : null
                sourceRect: Qt.rect(-Config.borderWidth, contentArea.height - (floatingStatusBar.height - Config.borderWidth) - floatingStatusBar.backdropPad, floatingStatusBar.width, floatingStatusBar.height + floatingStatusBar.backdropPad)
                textureSize: Qt.size(Math.max(1, Math.round(floatingStatusBar.width / 10)), Math.max(1, Math.round((floatingStatusBar.height + floatingStatusBar.backdropPad) / 10)))
            }

            MultiEffect {
                visible: Config.floatingStatusBar
                y: -floatingStatusBar.backdropPad
                width: floatingStatusBar.width
                height: floatingStatusBar.height + floatingStatusBar.backdropPad
                source: statusBarBackdrop
                autoPaddingEnabled: false
                blurEnabled: true
                blur: 1.0
                blurMax: 64

                layer.enabled: true
                layer.effect: MultiEffect {
                    autoPaddingEnabled: false
                    blurEnabled: true
                    blur: 1.0
                    blurMax: 64
                    maskEnabled: true
                    maskSource: statusBarBlurMask
                }
            }

            Rectangle {
                width: root._w
                height: root._h
                anchors.bottom: parent.bottom
                radius: root.cornerRadius
                color: Config.withAlpha(Theme.statusBarBackground, (Config.floatingStatusBar ? 0.78 : 1.0) * Config.windowOpacity)
            }

            ViciDivider {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: Config.borderWidth
                anchors.rightMargin: Config.borderWidth
            }

            Footer {
                id: footer
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.leftMargin: Config.borderWidth
                anchors.rightMargin: Config.borderWidth
                anchors.bottomMargin: Config.borderWidth
                height: 40
            }
        }

        Item {
            id: statusBarBlurMask
            width: floatingStatusBar.width
            height: floatingStatusBar.height + floatingStatusBar.backdropPad
            visible: false
            layer.enabled: true

            Rectangle {
                width: root._w
                height: root._h
                anchors.bottom: parent.bottom
                radius: root.cornerRadius
                color: "white"
            }
        }

        SourceBlendRect {
            visible: !Launcher.compacted && !root.nativeChrome
            anchors.fill: parent
            radius: root.cornerRadius
            overlay: true
            borderColor: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            borderWidth: Config.borderWidth
        }

        Loader {
            id: overlayLoader
            anchors.fill: parent
            anchors.margins: Config.borderWidth
            visible: Launcher.hasOverlay

            onLoaded: if (item)
                item.forceActiveFocus()
        }

        ActionPanelPopover {
            id: actionPanelPopover
            parent: footer
            controller: Launcher.actionPanel
            maxHeight: Math.round(root.height * 0.55)
        }

        ActionPanelPopover {
            id: footerMenuPopover
            parent: footer
            controller: Launcher.footerPanel
            alignLeft: true
            maxHeight: Math.round(root.height * 0.55)
        }

        MouseArea {
            id: modalScrim
            anchors.fill: parent
            z: 200
            enabled: Launcher.alertModel.visible
            visible: dim.opacity > 0
            hoverEnabled: true
            acceptedButtons: Qt.AllButtons
            onClicked: alertDialog.close()
            onWheel: function (wheel) {
                wheel.accepted = true;
            }

            Rectangle {
                id: dim
                anchors.fill: parent
                radius: Config.borderRounding
                color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.5)
                opacity: Launcher.alertModel.visible ? 1 : 0

                Behavior on opacity {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }

        AlertDialog {
            id: alertDialog
        }

        MouseArea {
            anchors.fill: parent
            z: 300
            enabled: Launcher.canPositionWindow
            acceptedButtons: Qt.LeftButton
            readonly property int topGrabHeight: 12
            onPressed: mouse => {
                if ((mouse.modifiers & Qt.ControlModifier) || mouse.y < topGrabHeight) {
                    Launcher.beginWindowDrag();
                } else {
                    mouse.accepted = false;
                }
            }
            onPositionChanged: Launcher.updateWindowDrag()
            onReleased: Launcher.endWindowDrag()
            onCanceled: Launcher.endWindowDrag()
        }
    }

    Window {
        id: anchorOverlay
        visible: Launcher.dragOverlayVisible
        x: Launcher.dragOverlayGeometry.x
        y: Launcher.dragOverlayGeometry.y
        width: Launcher.dragOverlayGeometry.width
        height: Launcher.dragOverlayGeometry.height
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.WindowTransparentForInput | Qt.WindowDoesNotAcceptFocus
        color: "transparent"

        Component.onCompleted: Launcher.registerDragOverlay(anchorOverlay)

        readonly property var activeAnchor: Launcher.dragActiveAnchor >= 0 ? Launcher.dragAnchors[Launcher.dragActiveAnchor] : null

        Repeater {
            model: Launcher.dragGuideXs

            GuideLine {
                required property var modelData
                x: modelData
                vertical: true
                active: anchorOverlay.activeAnchor !== null && anchorOverlay.activeAnchor.x === modelData
            }
        }

        Repeater {
            model: Launcher.dragGuideYs

            GuideLine {
                required property var modelData
                y: modelData
                active: anchorOverlay.activeAnchor !== null && anchorOverlay.activeAnchor.y === modelData
            }
        }
    }

    component GuideLine: Shape {
        id: guide
        property bool active: false
        property bool vertical: false

        ShapePath {
            strokeStyle: ShapePath.DashLine
            dashPattern: [3, 3]
            strokeWidth: 2
            strokeColor: guide.active ? Theme.accent : Config.withAlpha(Theme.foreground, 0.45)
            fillColor: "transparent"
            startX: 0
            startY: 0

            PathLine {
                x: guide.vertical ? 0 : anchorOverlay.width
                y: guide.vertical ? anchorOverlay.height : 0
            }
        }
    }

    Connections {
        target: Launcher.alertModel
        function onVisibleChanged() {
            if (Launcher.alertModel.visible) {
                alertDialog.open();
            } else {
                if (alertDialog.visible)
                    alertDialog.close();
                searchBar.focusInput();
            }
        }
    }

    Connections {
        target: Launcher
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
            if (Launcher.hasOverlay) {
                overlayLoader.setSource(Launcher.overlayUrl, {
                    host: Launcher.overlayHost
                });
            } else {
                overlayLoader.source = "";
                searchBar.focusInput();
            }
        }
    }

    Connections {
        target: Launcher.nav
        function onWindowVisiblityChanged(visible) {
            if (visible) {
                root.aboutToShow();
                if (root.autoPlaceOnShow && !Launcher.restoreWindowPosition())
                    Launcher.positionOnCursorScreen();
                root.visible = true;
                root.raise();
                root.requestActivate();
                searchBar.focusInput();
                root.shown();
            } else {
                root.visible = false;
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        enabled: !Launcher.alertModel.visible && !Launcher.actionPanel.open && !Launcher.footerPanel.open && !Launcher.hasOverlay
        onActivated: Launcher.handleEscape()
    }

    Shortcut {
        sequence: "Shift+Escape"
        enabled: !Launcher.alertModel.visible
        onActivated: Launcher.popToRoot()
    }

    Shortcut {
        sequence: Keybinds.toggleActionPanelSequence
        enabled: !Launcher.alertModel.visible
        onActivated: {
            if (Launcher.compacted)
                Launcher.expand();
            Launcher.actionPanel.toggle();
        }
    }

    onWidthChanged: {
        if (Launcher.canPositionWindow && root.autoPlaceOnShow)
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
    }
    onHeightChanged: {
        if (Launcher.canPositionWindow && root.autoPlaceOnShow)
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
    }

    Component.onCompleted: {
        if (Launcher.canPositionWindow && root.autoPlaceOnShow) {
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
            Launcher.positionOnCursorScreen();
        }
    }
}
