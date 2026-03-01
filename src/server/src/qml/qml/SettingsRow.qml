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
        Layout.topMargin: 12
        Layout.bottomMargin: 12
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
            Layout.preferredWidth: 250
            Layout.minimumWidth: 200
            implicitHeight: children.length > 0 ? children[0].implicitHeight : 0
            onChildrenChanged: _bindChildrenWidth()
            onWidthChanged: _bindChildrenWidth()
            function _bindChildrenWidth() {
                for (let i = 0; i < children.length; i++)
                    children[i].width = Qt.binding(() => controlSlot.width)
            }
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
