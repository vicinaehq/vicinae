import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 0
    Layout.fillWidth: true

    property string label: ""
    property string description: ""
    property var iconSource
    property bool showSeparator: true
    property real controlWidth: 200

    readonly property real _hPadding: 16
    readonly property real _vPadding: description !== "" ? 13 : 11
    default property alias contentData: controlSlot.data

    RowLayout {
        Layout.fillWidth: true
        Layout.leftMargin: root._hPadding
        Layout.rightMargin: root._hPadding
        Layout.topMargin: root._vPadding
        Layout.bottomMargin: root._vPadding
        spacing: 20

        ViciImage {
            visible: root.iconSource !== undefined
            source: visible ? root.iconSource : ""
            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
        }

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
            onChildrenChanged: {
                for (var i = 0; i < children.length; i++) {
                    if (children[i].accessibleLabel !== undefined)
                        children[i].accessibleLabel = Qt.binding(function () {
                            return root.label;
                        });
                }
            }
        }
    }

    ViciDivider {
        visible: root.showSeparator
        Layout.fillWidth: true
        Layout.leftMargin: root._hPadding
        Layout.rightMargin: root._hPadding
    }
}
