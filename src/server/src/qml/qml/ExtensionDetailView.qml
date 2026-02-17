import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    required property string markdown
    required property var metadata

    spacing: 0

    MarkdownText {
        Layout.fillWidth: true
        Layout.fillHeight: true
        markdown: root.markdown
        contentPadding: 20
        focus: true
        Component.onCompleted: forceActiveFocus()
    }

    Rectangle {
        visible: root.metadata.length > 0
        Layout.fillHeight: true
        implicitWidth: 1
        color: Theme.divider
    }

    MetadataBar {
        visible: root.metadata.length > 0
        Layout.preferredWidth: root.width * 0.40
        Layout.fillHeight: true
        model: root.metadata
    }
}
