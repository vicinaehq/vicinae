pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

ColumnLayout {
    id: root
    spacing: 4
    Layout.fillWidth: true

    property string label: ""
    property string error: ""
    property string info: ""
    // Label above the field instead of beside it, for forms laid out in columns.
    property bool stacked: false
    readonly property real stackedLabelHeight: stackedLabel.visible ? stackedLabel.height + spacing : 0
    property bool topAlignLabel: false
    property real topAlignLabelTopPadding: 8
    property bool filled: true
    property Item focusTarget: null

    function focusField() {
        focusTarget?.forceActiveFocus();
    }
    default property alias contentData: contentSlot.data

    Text {
        id: stackedLabel
        visible: root.stacked && root.label !== ""
        Layout.fillWidth: true
        text: root.label
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 20

        Text {
            visible: !root.stacked
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
            onChildrenChanged: _applyChildProps()
            onWidthChanged: _applyChildProps()
            function _applyChildProps() {
                for (let i = 0; i < children.length; i++) {
                    children[i].width = Qt.binding(function () {
                        return contentSlot.width;
                    });
                    if (children[i].filled !== undefined)
                        children[i].filled = Qt.binding(function () {
                            return root.filled;
                        });
                    if (children[i].accessibleLabel !== undefined)
                        children[i].accessibleLabel = Qt.binding(function () {
                            return root.label;
                        });
                }
            }
        }

        Item {
            visible: !root.stacked
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }

    RowLayout {
        visible: root.error !== ""
        Layout.fillWidth: true
        spacing: 20

        Item {
            visible: !root.stacked
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
            visible: !root.stacked
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }

    RowLayout {
        visible: root.info !== ""
        Layout.fillWidth: true
        spacing: 20

        Item {
            visible: !root.stacked
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }

        Text {
            Layout.preferredWidth: 5
            Layout.fillWidth: true
            text: root.info
            textFormat: Text.StyledText
            color: Theme.textMuted
            linkColor: Theme.accent
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            onLinkActivated: link => Qt.openUrlExternally(link)

            MouseArea {
                anchors.fill: parent
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                acceptedButtons: Qt.NoButton
            }
        }

        Item {
            visible: !root.stacked
            Layout.preferredWidth: 2
            Layout.fillWidth: true
        }
    }
}
