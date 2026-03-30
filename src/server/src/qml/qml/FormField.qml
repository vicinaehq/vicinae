import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 4
    Layout.fillWidth: true

    property string label: ""
    property string error: ""
    property string info: ""
    property bool topAlignLabel: false
    property real topAlignLabelTopPadding: 8
    default property alias contentData: contentSlot.data

    RowLayout {
        Layout.fillWidth: true
        spacing: 20

        Text {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
            Layout.topMargin: root.topAlignLabel ? root.topAlignLabelTopPadding : 0
            text: root.label
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            horizontalAlignment: Text.AlignRight
            verticalAlignment: root.topAlignLabel ? Text.AlignTop : Text.AlignVCenter
            Layout.alignment: root.topAlignLabel ? Qt.AlignTop : Qt.AlignVCenter
        }

        Item {
            id: contentSlot
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            implicitHeight: children.length > 0 ? children[0].implicitHeight : 0
            onChildrenChanged: _bindChildrenWidth()
            onWidthChanged: _bindChildrenWidth()
            function _bindChildrenWidth() {
                for (var i = 0; i < children.length; i++)
                    children[i].width = Qt.binding(function () {
                        return contentSlot.width;
                    });
            }
        }

        Item {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }

    RowLayout {
        visible: root.error !== ""
        Layout.fillWidth: true
        spacing: 20

        Item {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }

        Text {
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            text: root.error
            color: Theme.danger
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
        }

        Item {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }

    RowLayout {
        visible: root.info !== ""
        Layout.fillWidth: true
        spacing: 20

        Item {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }

        Text {
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            text: root.info
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
        }

        Item {
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }
}
