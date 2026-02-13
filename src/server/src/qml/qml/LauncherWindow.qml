import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

Window {
    id: root
    width: 770
    height: 480
    minimumWidth: 770
    maximumWidth: 770
    minimumHeight: 480
    maximumHeight: 480
    title: "Vicinae Launcher"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"
    visible: false

    Rectangle {
        id: frame
        anchors.fill: parent
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.92)
        radius: 10
        border.color: Theme.mainWindowBorder
        border.width: 1

        // Clip children to the rounded rectangle
        layer.enabled: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 1
            spacing: 0

            SearchBar {
                id: searchBar
                Layout.fillWidth: true
                Layout.preferredHeight: 60
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Theme.divider
            }

            // Content area — holds QML root search list, QML command views, or embedded widget views
            Item {
                id: contentArea
                objectName: "contentArea"
                Layout.fillWidth: true
                Layout.fillHeight: true

                RootSearchList {
                    id: searchList
                    anchors.fill: parent
                    visible: !launcher.hasCommandView
                }

                StackView {
                    id: commandStack
                    anchors.fill: parent
                    visible: launcher.hasCommandView
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Theme.divider
            }

            Footer {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
            }
        }
    }

    Component {
        id: commandListComponent
        CommandListView {}
    }

    Connections {
        target: launcher
        function onCommandViewPushed(model) {
            commandStack.push(commandListComponent, { "cmdModel": model }, StackView.Immediate)
        }
        function onCommandViewPopped() {
            if (commandStack.depth > 0)
                commandStack.pop(null, StackView.Immediate)
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
        onActivated: launcher.goBack()
    }

    Shortcut {
        sequence: "Shift+Escape"
        onActivated: launcher.popToRoot()
    }

    // Center on screen
    Component.onCompleted: {
        root.x = (Screen.width - root.width) / 2
        root.y = (Screen.height - root.height) / 3
    }
}
