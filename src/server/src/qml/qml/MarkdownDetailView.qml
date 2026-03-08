import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    required property string markdown
    property var metadata: []

    readonly property bool _hasMarkdown: root.markdown !== ""
    readonly property bool _hasMetadata: root.metadata.length > 0

    function moveUp() { mdContent.scrollUp() }
    function moveDown() { mdContent.scrollDown() }
    function moveSectionUp() { moveUp() }
    function moveSectionDown() { moveDown() }

    spacing: 0

    MarkdownText {
        id: mdContent
        visible: root._hasMarkdown
        Layout.fillWidth: true
        Layout.fillHeight: true
        markdown: root.markdown
        contentPadding: 20
    }

    Rectangle {
        visible: root._hasMarkdown && root._hasMetadata
        Layout.fillHeight: true
        implicitWidth: 1
        color: Theme.divider
    }

    MetadataBar {
        visible: root._hasMetadata
        Layout.preferredWidth: root._hasMarkdown ? root.width * 0.40 : -1
        Layout.fillWidth: !root._hasMarkdown
        Layout.fillHeight: true
        model: root.metadata
    }
}
