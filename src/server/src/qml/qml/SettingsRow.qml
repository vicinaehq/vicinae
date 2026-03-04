import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 0
    Layout.fillWidth: true

    property string label: ""
    property string description: ""
    property bool showSeparator: true
    default property alias contentData: controlSlot.data

    RowLayout {
        Layout.fillWidth: true
        Layout.leftMargin: 20
        Layout.rightMargin: 20
        Layout.topMargin: root.description !== "" ? 12 : 8
        Layout.bottomMargin: root.description !== "" ? 12 : 8
        spacing: 20

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: root.label
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                Layout.fillWidth: true
            }

            Text {
                visible: root.description !== ""
                text: root.description
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }

        Item {
            id: controlSlot
            Layout.preferredWidth: 180
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: children.length > 0 ? children[0].implicitHeight : 0
        }
    }

    Rectangle {
        visible: root.showSeparator
        Layout.fillWidth: true
        Layout.leftMargin: 20
        Layout.rightMargin: 20
        height: 1
        color: Theme.divider
    }
}
