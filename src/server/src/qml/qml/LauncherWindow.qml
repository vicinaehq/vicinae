import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    id: root
    readonly property int _w: launcher.overrideWidth || Config.windowWidth
    readonly property int _h: launcher.overrideHeight || Config.windowHeight
    width: _w
    height: _h
    minimumWidth: _w
    maximumWidth: _w
    minimumHeight: _h
    maximumHeight: _h
    title: "Vicinae Launcher"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    Rectangle {
        visible: launcher.compacted
        width: root.width
        height: 60 + 2 * Config.borderWidth
        radius: Config.borderRounding
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        border.color: Theme.mainWindowBorder
        border.width: Config.borderWidth
    }

    Item {
        visible: !launcher.compacted
        anchors.fill: parent
        anchors.bottomMargin: launcher.hasOverlay ? 0 : footer.height + Config.borderWidth
        clip: true

        Rectangle {
            width: root.width
            height: root.height
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
            width: root.width
            height: root.height
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

            RootSearchList {
                id: searchList
                anchors.fill: parent
                visible: !launcher.hasCommandView && !launcher.compacted
            }

            StackView {
                id: commandStack
                anchors.fill: parent
                visible: launcher.hasCommandView && !launcher.compacted
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

        onLoaded: if (item) item.forceActiveFocus()
    }

    ActionPanelPopover {
        id: actionPanelPopover
        z: 100
        anchors.fill: parent
        anchors.bottomMargin: footer.height + 1 + Config.borderWidth
    }

    AlertDialog {
        id: alertDialog
    }

    Connections {
        target: launcher.alertModel
        function onVisibleChanged() {
            if (launcher.alertModel.visible) {
                alertDialog.open()
            } else {
                if (alertDialog.visible)
                    alertDialog.close()
                searchBar.focusInput()
            }
        }
    }

    Connections {
        target: launcher
        function onCommandViewPushed(componentUrl, properties) {
            commandStack.push(componentUrl, properties, StackView.Immediate)
        }
        function onCommandViewPopped() {
            if (commandStack.depth > 0)
                commandStack.pop(StackView.Immediate)
        }
        function onCommandStackCleared() {
            commandStack.clear(StackView.Immediate)
        }
        function onOverlayChanged() {
            if (launcher.hasOverlay) {
                overlayLoader.setSource(launcher.overlayUrl, { host: launcher.overlayHost })
            } else {
                overlayLoader.source = ""
                searchBar.focusInput()
            }
        }
    }

    Connections {
        target: Nav
        function onWindowVisiblityChanged(visible) {
            if (visible) {
                root.visible = true
                root.raise()
                root.requestActivate()
                searchBar.focusInput()
            } else {
                root.visible = false
            }
        }
    }


    Shortcut {
        sequence: "Escape"
        enabled: !launcher.alertModel.visible && !actionPanel.open && !launcher.hasOverlay
        onActivated: launcher.goBack()
    }

    Shortcut {
        sequence: "Shift+Escape"
        enabled: !launcher.alertModel.visible
        onActivated: launcher.popToRoot()
    }

    Shortcut {
        sequence: "Ctrl+B"
        enabled: !launcher.alertModel.visible
        onActivated: {
            if (launcher.compacted) launcher.expand()
            actionPanel.toggle()
        }
    }

    Connections {
        target: actionPanel
        function onOpenChanged() {
            if (!actionPanel.open) {
                searchBar.focusInput()
            }
        }
    }

    onWidthChanged: root.x = (Screen.width - root.width) / 2
    onHeightChanged: root.y = (Screen.height - root.height) / 3

    Component.onCompleted: {
        root.x = (Screen.width - root.width) / 2
        root.y = (Screen.height - root.height) / 3
    }
}
