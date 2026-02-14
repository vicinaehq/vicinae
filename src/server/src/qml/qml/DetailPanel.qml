import QtQuick
import QtQuick.Layouts

/// Reusable detail panel with arbitrary content on top and optional metadata at the bottom.
/// Place child items inside to fill the content area. Set `metadata` to show a MetadataBar.
Item {
    id: root
    default property alias content: contentArea.data
    property var metadata: []

    readonly property bool _hasMetadata: root.metadata.length > 0

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            visible: root._hasMetadata
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

        MetadataBar {
            id: metadataBar
            visible: root._hasMetadata
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(implicitHeight, root.height * 0.4)
            model: root.metadata
        }
    }
}
