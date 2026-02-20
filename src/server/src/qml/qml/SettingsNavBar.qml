import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: 60

    property int currentIndex: 0
    signal tabClicked(int index)

    readonly property var _tabs: [
        { icon: "cog", label: "General" },
        { icon: "computer-chip", label: "Extensions" },
        { icon: "keyboard", label: "Keybinds" },
        { icon: "vicinae", label: "About" }
    ]

    RowLayout {
        anchors.centerIn: parent
        spacing: 4

        Repeater {
            model: root._tabs
            delegate: Rectangle {
                id: pane
                required property int index
                required property var modelData
                width: 100
                height: 50
                radius: 6
                color: root.currentIndex === index ? Theme.listItemSelectionBg
                       : hoverHandler.hovered ? Theme.listItemHoverBg
                       : "transparent"

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 4

                    ViciImage {
                        source: Img.builtin(pane.modelData.icon).withFillColor(
                            root.currentIndex === pane.index ? Theme.foreground : Theme.textMuted)
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: pane.modelData.label
                        color: root.currentIndex === pane.index ? Theme.foreground : Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }

                HoverHandler { id: hoverHandler }
                TapHandler {
                    onTapped: root.tabClicked(pane.index)
                }
            }
        }
    }
}
