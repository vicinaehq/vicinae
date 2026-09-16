pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Shapes
import Vicinae

LauncherWindowBase {
    id: root

    property int cornerRadius: Config.borderRounding
    property bool blurEnabled: Config.blurEnabled
    property bool shadowEnabled: shadowPadding > 0
    property bool nativeChrome: false
    property bool autoPlaceOnShow: true
    property Component contentEffect: null
    property Component searchBarComponent: SearchBar {
        commandStack: root.commandStack
    }
    property Component statusBarComponent: LauncherStatusBar {
        backdrop: root.popupBackdrop
        windowRadius: root.cornerRadius
        windowHeight: root._h
        windowWidth: root._w
    }
    readonly property alias commandStack: commandStack
    statusBarOverlap: floatingStatusBar.visible && root.appearance.floatingStatusBar ? floatingStatusBar.height - root.appearance.contentInset : 0
    statusBarTop: shadowPadding + floatingStatusBar.y
    popupBackdrop: contentArea
    signal aboutToShow
    signal shown

    readonly property int _w: Launcher.overrideWidth || Config.windowWidth
    readonly property int _h: Launcher.overrideHeight || Config.windowHeight
    readonly property int _contentH: Launcher.compacted ? root.appearance.searchBarHeight + 2 * root.appearance.contentInset : root._h

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
            height: root.appearance.searchBarHeight + 2 * root.appearance.contentInset
            radius: root.cornerRadius
            color: root.appearance.windowBackground
        }

        SourceBlendRect {
            visible: Launcher.compacted && !root.nativeChrome
            width: root._w
            height: root.appearance.searchBarHeight + 2 * root.appearance.contentInset
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
            color: root.appearance.windowBackground
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: root.appearance.contentInset
            spacing: 0
            visible: !Launcher.hasOverlay

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: root.searchBarOverlap > 0 ? 0 : root.appearance.searchBarHeight
                visible: Launcher.searchVisible
            }

            HorizontalLoadingBar {
                Layout.fillWidth: true
                implicitHeight: Launcher.searchVisible ? 1 : 0
                visible: Launcher.searchVisible && !Launcher.compacted
                loading: Launcher.isLoading
                dividerVisible: root.appearance.searchDividerVisible
            }

            Item {
                id: contentViewport
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    anchors.fill: parent
                    anchors.bottomMargin: (root.contentEffect === null || !root.appearance.floatingStatusBar) && floatingStatusBar.visible ? floatingStatusBar.height - root.appearance.contentInset : 0
                    clip: true

                    Item {
                        id: contentArea
                        objectName: "contentArea"
                        width: contentViewport.width
                        height: root.appearance.floatingStatusBar ? contentViewport.height : parent.height
                        layer.enabled: root.contentEffect !== null
                        layer.effect: root.contentEffect

                        StackView {
                            id: commandStack
                            anchors.fill: parent
                            visible: !Launcher.compacted
                        }
                    }
                }
            }
        }

        Loader {
            id: searchBar
            x: root.appearance.contentInset
            y: root.appearance.contentInset
            width: parent.width - 2 * root.appearance.contentInset
            height: root.appearance.searchBarHeight
            visible: Launcher.searchVisible && !Launcher.hasOverlay
            enabled: !Launcher.alertModel.visible
            sourceComponent: root.searchBarComponent

            function focusInput() {
                (item as SearchBar)?.focusInput();
            }
        }

        Loader {
            id: floatingStatusBar
            visible: !Launcher.compacted && !Launcher.hasOverlay && Launcher.statusBarVisible
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: (item as Item)?.implicitHeight ?? 0
            sourceComponent: root.statusBarComponent
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
            anchors.margins: root.appearance.contentInset
            visible: Launcher.hasOverlay

            onLoaded: (item as Item)?.forceActiveFocus()
        }

        ActionPanelPopover {
            id: actionPanelPopover
            parent: (floatingStatusBar.item as LauncherStatusBar)?.popupAnchor ?? floatingStatusBar
            controller: Launcher.actionPanel
            maxHeight: Math.round(root.height * 0.55)
        }

        ActionPanelPopover {
            id: footerMenuPopover
            parent: (floatingStatusBar.item as LauncherStatusBar)?.popupAnchor ?? floatingStatusBar
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
