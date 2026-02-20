import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    id: root
    width: Config.windowWidth
    height: Config.windowHeight
    minimumWidth: Config.windowWidth
    maximumWidth: Config.windowWidth
    minimumHeight: Config.windowHeight
    maximumHeight: Config.windowHeight
    title: "Vicinae Launcher"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    // Compact mode background — only covers search bar area
    Rectangle {
        visible: launcher.compacted
        width: root.width
        height: 60 + 2 * Config.borderWidth
        radius: Config.borderRounding
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        border.color: Theme.mainWindowBorder
        border.width: Config.borderWidth
    }

    // Main background — clipped to the region above the footer
    Item {
        visible: !launcher.compacted
        anchors.fill: parent
        anchors.bottomMargin: footer.height + Config.borderWidth
        clip: true

        Rectangle {
            width: root.width
            height: root.height
            radius: Config.borderRounding
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        }
    }

    // Footer background — clipped to the footer region at the bottom
    Item {
        visible: !launcher.compacted
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

    // Border frame
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
            visible: !launcher.compacted
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

        Footer {
            id: footer
            visible: !launcher.compacted
            Layout.fillWidth: true
            Layout.preferredHeight: launcher.compacted ? 0 : 40
        }
    }

    // Action panel popover overlay — anchored above the footer
    ActionPanelPopover {
        id: actionPanelPopover
        visible: !launcher.compacted
        z: 100
        anchors.fill: parent
        anchors.bottomMargin: footer.height + 1 + Config.borderWidth
    }

    // Alert dialog
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
        enabled: !launcher.alertModel.visible && !actionPanel.open
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

    Component.onCompleted: {
        root.x = (Screen.width - root.width) / 2
        root.y = (Screen.height - root.height) / 3
    }
}
