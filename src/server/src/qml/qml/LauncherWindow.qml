import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Shapes

Window {
    id: root
    property int shadowPadding: 0

    property int cornerRadius: Config.borderRounding
    property bool blurEnabled: Config.blurEnabled
    property bool shadowEnabled: shadowPadding > 0
    property bool nativeChrome: false
    property bool autoPlaceOnShow: true
    readonly property int statusBarOverlap: floatingStatusBar.visible ? floatingStatusBar.height - Config.borderWidth : 0
    readonly property real statusBarTop: shadowPadding + floatingStatusBar.y
    readonly property Item popupBackdrop: contentArea
    signal aboutToShow
    signal shown

    readonly property int _w: launcher.overrideWidth || Config.windowWidth
    readonly property int _h: launcher.overrideHeight || Config.windowHeight
    readonly property int _contentH: launcher.compacted ? 60 + 2 * Config.borderWidth : _h

    width: _w + 2 * shadowPadding
    height: _h + 2 * shadowPadding
    minimumWidth: _w + 2 * shadowPadding
    maximumWidth: _w + 2 * shadowPadding
    minimumHeight: _h + 2 * shadowPadding
    maximumHeight: _h + 2 * shadowPadding
    title: qsTr("Vicinae Launcher")
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    WindowMaterial.enabled: root.blurEnabled && !root.nativeChrome
    WindowMaterial.radius: root.cornerRadius
    WindowMaterial.region: Qt.rect(shadowPadding, shadowPadding, _w, launcher.compacted ? _contentH : _h)

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

        RectangularShadow {
            x: root.shadowPadding
            y: root.shadowPadding
            width: _w
            height: _contentH
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
        width: _w
        height: _h

        Rectangle {
            visible: launcher.compacted
            width: _w
            height: 60 + 2 * Config.borderWidth
            radius: root.cornerRadius
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        }

        SourceBlendRect {
            visible: launcher.compacted && !root.nativeChrome
            width: _w
            height: 60 + 2 * Config.borderWidth
            radius: root.cornerRadius
            overlay: true
            borderColor: Config.withAlpha(Theme.mainWindowBorder, Config.windowOpacity)
            borderWidth: Config.borderWidth
        }

        Rectangle {
            visible: !launcher.compacted
            width: _w
            height: _h
            radius: root.cornerRadius
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
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
                        height: contentViewport.height

                        StackView {
                            id: commandStack
                            anchors.fill: parent
                            visible: !launcher.compacted
                        }
                    }
                }
            }
        }

        Item {
            id: floatingStatusBar
            visible: !launcher.compacted && !launcher.hasOverlay && launcher.statusBarVisible
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 41 + Config.borderWidth
            clip: true

            readonly property int backdropPad: 64

            ShaderEffectSource {
                id: statusBarBackdrop
                visible: false
                sourceItem: contentArea
                sourceRect: Qt.rect(-Config.borderWidth, contentArea.height - (floatingStatusBar.height - Config.borderWidth) - floatingStatusBar.backdropPad, floatingStatusBar.width, floatingStatusBar.height + floatingStatusBar.backdropPad)
                textureSize: Qt.size(Math.max(1, Math.round(floatingStatusBar.width / 10)), Math.max(1, Math.round((floatingStatusBar.height + floatingStatusBar.backdropPad) / 10)))
            }

            MultiEffect {
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
                width: _w
                height: _h
                anchors.bottom: parent.bottom
                radius: root.cornerRadius
                color: Config.withAlpha(Theme.statusBarBackground, 0.78 * Config.windowOpacity)
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
                width: _w
                height: _h
                anchors.bottom: parent.bottom
                radius: root.cornerRadius
                color: "white"
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
            parent: footer
            controller: actionPanel
            maxHeight: Math.round(root.height * 0.55)
        }

        ActionPanelPopover {
            id: footerMenuPopover
            parent: footer
            controller: footerPanel
            alignLeft: true
            maxHeight: Math.round(root.height * 0.55)
        }

        MouseArea {
            id: modalScrim
            anchors.fill: parent
            z: 200
            enabled: launcher.alertModel.visible
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
                opacity: launcher.alertModel.visible ? 1 : 0

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
            enabled: launcher.canPositionWindow
            acceptedButtons: Qt.LeftButton
            onPressed: mouse => {
                if (mouse.modifiers & Qt.ControlModifier) {
                    launcher.beginWindowDrag();
                } else {
                    mouse.accepted = false;
                }
            }
            onPositionChanged: launcher.updateWindowDrag()
            onReleased: launcher.endWindowDrag()
            onCanceled: launcher.endWindowDrag()
        }
    }

    Window {
        id: anchorOverlay
        visible: launcher.dragOverlayVisible
        x: launcher.dragOverlayGeometry.x
        y: launcher.dragOverlayGeometry.y
        width: launcher.dragOverlayGeometry.width
        height: launcher.dragOverlayGeometry.height
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.WindowTransparentForInput | Qt.WindowDoesNotAcceptFocus
        color: "transparent"

        Component.onCompleted: launcher.registerDragOverlay(anchorOverlay)

        readonly property var activeAnchor: launcher.dragActiveAnchor >= 0 ? launcher.dragAnchors[launcher.dragActiveAnchor] : null

        Repeater {
            model: launcher.dragGuideXs

            GuideLine {
                required property var modelData
                x: modelData
                vertical: true
                active: anchorOverlay.activeAnchor !== null && anchorOverlay.activeAnchor.x === modelData
            }
        }

        Repeater {
            model: launcher.dragGuideYs

            GuideLine {
                required property var modelData
                y: modelData
                active: anchorOverlay.activeAnchor !== null && anchorOverlay.activeAnchor.y === modelData
            }
        }
    }

    component GuideLine: Shape {
        property bool active: false
        property bool vertical: false

        ShapePath {
            strokeStyle: ShapePath.DashLine
            dashPattern: [3, 3]
            strokeWidth: 2
            strokeColor: active ? Theme.accent : Config.withAlpha(Theme.foreground, 0.45)
            fillColor: "transparent"
            startX: 0
            startY: 0

            PathLine {
                x: vertical ? 0 : anchorOverlay.width
                y: vertical ? anchorOverlay.height : 0
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

    Connections {
        target: Nav
        function onWindowVisiblityChanged(visible) {
            if (visible) {
                root.aboutToShow();
                if (root.autoPlaceOnShow && !launcher.restoreWindowPosition())
                    launcher.positionOnCursorScreen();
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
        if (launcher.canPositionWindow && root.autoPlaceOnShow)
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
    }
    onHeightChanged: {
        if (launcher.canPositionWindow && root.autoPlaceOnShow)
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
    }

    Component.onCompleted: {
        if (launcher.canPositionWindow && root.autoPlaceOnShow) {
            root.x = Screen.virtualX + (Screen.width - root.width) / 2;
            root.y = Screen.virtualY + (Screen.height - root.height) / 3;
            launcher.positionOnCursorScreen();
        }
    }
}
