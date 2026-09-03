import QtQuick
import QtQuick.Controls

Row {
    id: root

    spacing: 2

    Repeater {
        model: bar.tray

        BarPill {
            id: pill
            required property string key
            required property string hint
            required property var icon
            required property bool attention
            required property bool hasMenu
            required property bool isMenu

            horizontalPadding: 5
            tooltip: pill.hint
            highlighted: menu.visible

            onClicked: mouse => {
                const wantsMenu = mouse.button === Qt.RightButton || pill.isMenu;
                if (wantsMenu && pill.hasMenu) {
                    bar.tray.openMenu(pill.key);
                    menu.open();
                } else if (mouse.button === Qt.RightButton) {
                    bar.tray.secondaryActivate(pill.key);
                } else if (mouse.button === Qt.LeftButton) {
                    bar.tray.activate(pill.key);
                }
            }

            Item {
                width: 20
                height: 20

                ViciImage {
                    anchors.fill: parent
                    source: pill.icon.withTemplateFill(Theme.foreground)
                    sourceSize: Qt.size(20, 20)
                }

                Rectangle {
                    visible: pill.attention
                    width: 6
                    height: 6
                    radius: 3
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.topMargin: -2
                    anchors.rightMargin: -2
                    color: Theme.toastWarning
                }
            }

            BarTrayMenu {
                id: menu
                anchorItem: pill
                onClosed: bar.tray.closeMenu()
            }
        }
    }
}
