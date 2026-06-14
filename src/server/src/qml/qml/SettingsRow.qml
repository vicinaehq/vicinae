import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 0
    Layout.fillWidth: true

    property string label: ""
    property string description: ""
    property bool showSeparator: true
    property real hPadding: 16
    property real controlWidth: 200
    default property alias contentData: controlSlot.data

    RowLayout {
        Layout.fillWidth: true
        Layout.leftMargin: root.hPadding
        Layout.rightMargin: root.hPadding
        Layout.topMargin: root.description !== "" ? 11 : 9
        Layout.bottomMargin: root.description !== "" ? 11 : 9
        spacing: 20

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 3

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
            Layout.preferredWidth: root.controlWidth
            Layout.alignment: Qt.AlignVCenter
            implicitHeight: children.length > 0 ? children[0].implicitHeight : 0
        }
    }

    ViciDivider {
        visible: root.showSeparator
        Layout.fillWidth: true
        Layout.leftMargin: root.hPadding
        Layout.rightMargin: root.hPadding
    }
}
