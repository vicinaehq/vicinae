import QtQuick
import QtQuick.Controls

BarPopover {
    id: root

    readonly property var menuModel: bar.tray.menu

    minimumWidth: 240

    contentItem: Column {
        id: column
        width: root.minimumWidth
        spacing: 0

        BarMenuRow {
            visible: root.menuModel.canGoBack
            width: parent.width
            label: root.menuModel.title
            iconSource: Img.builtin("chevron-left").withFillColor(Theme.textMuted)
            showIconSlot: true
            onClicked: root.menuModel.back()
        }

        Rectangle {
            visible: root.menuModel.canGoBack
            width: parent.width
            height: 1
            color: Theme.divider
        }

        Text {
            visible: bar.tray.menuLoading && list.count === 0
            text: qsTr("Loading…")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            padding: 8
        }

        ListView {
            id: list
            width: parent.width
            height: Math.min(contentHeight, 480)
            clip: true
            interactive: contentHeight > height
            model: root.menuModel
            boundsBehavior: Flickable.StopAtBounds

            delegate: Item {
                id: row
                required property int index
                required property int menuId
                required property string label
                required property bool menuEnabled
                required property bool separator
                required property bool submenu
                required property int toggleType
                required property bool checked
                required property bool hasIcon
                required property var icon

                width: list.width
                height: separator ? 9 : 28

                Rectangle {
                    visible: row.separator
                    anchors.centerIn: parent
                    width: parent.width - 12
                    height: 1
                    color: Theme.divider
                }

                BarMenuRow {
                    id: menuRow
                    visible: !row.separator
                    anchors.fill: parent
                    label: row.label
                    enabled: row.menuEnabled
                    submenu: row.submenu
                    toggleType: row.toggleType
                    checked: row.checked
                    showIconSlot: row.hasIcon
                    iconSource: row.hasIcon ? row.icon : undefined
                    onClicked: {
                        if (row.submenu) {
                            root.menuModel.enter(row.index);
                            return;
                        }
                        bar.tray.triggerMenuItem(row.menuId);
                        if (row.toggleType === 0)
                            root.close();
                    }
                }
            }
        }
    }
}
