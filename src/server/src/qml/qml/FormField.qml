import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 4
    Layout.fillWidth: true

    property string label: ""
    property string error: ""
    property string info: ""
    default property alias contentData: contentSlot.data

    RowLayout {
        Layout.fillWidth: true
        spacing: 20

        // Label — right-aligned, 2/9 of width
        Text {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
            text: root.label
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            Layout.alignment: Qt.AlignVCenter
        }

        // Input slot — 5/9 of width
        Item {
            id: contentSlot
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            implicitHeight: children.length > 0 ? children[0].implicitHeight : 0
            onChildrenChanged: _bindChildrenWidth()
            onWidthChanged: _bindChildrenWidth()
            function _bindChildrenWidth() {
                for (var i = 0; i < children.length; i++)
                    children[i].width = Qt.binding(function() { return contentSlot.width })
            }
        }

        // Error — left-aligned, 2/9 of width
        Text {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
            text: root.error
            color: Theme.danger
            font.pointSize: Theme.smallerFontSize
            verticalAlignment: Text.AlignVCenter
            Layout.alignment: Qt.AlignVCenter
        }
    }

    // Info text — below, aligned under the input column
    RowLayout {
        visible: root.info !== ""
        Layout.fillWidth: true
        spacing: 20

        Item { Layout.preferredWidth: 2; Layout.fillWidth: true }

        Text {
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            text: root.info
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
        }

        Item { Layout.preferredWidth: 2; Layout.fillWidth: true }
    }
}
