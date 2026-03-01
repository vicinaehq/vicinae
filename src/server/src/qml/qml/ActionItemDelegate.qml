import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: 36

    property bool selected: false
    readonly property bool hovered: mouseArea.containsMouse

    required property string title
    required property string iconSource
    required property string shortcutLabel
    required property bool isSubmenu
    required property bool isDanger

    signal clicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        color: {
            if (root.selected) return Theme.listItemSelectionBg
            if (root.hovered) return Theme.listItemHoverBg
            return "transparent"
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        Item {
            visible: root.iconSource !== ""
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            Layout.alignment: Qt.AlignVCenter

            ViciImage {
                anchors.fill: parent
                source: root.iconSource
            }
        }

        Text {
            text: root.title
            color: {
                if (root.isDanger) return Theme.danger
                if (root.selected) return Theme.listItemSelectionFg
                return Theme.foreground
            }
            font.pointSize: Theme.regularFontSize
            elide: Text.ElideRight
            maximumLineCount: 1
            Layout.fillWidth: true
        }

        ShortcutBadge {
            visible: root.shortcutLabel !== ""
            text: root.shortcutLabel
            Layout.alignment: Qt.AlignVCenter
        }

    }
}
