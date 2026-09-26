pragma ComponentBehavior: Bound
import QtQuick
import Vicinae.Documents

Item {
    id: root
    property DocumentStyle style: root.DocumentScope.style
    property Component imageDelegate: null

    property string markdown: ""
    property string fontFamily: ""
    implicitHeight: col.implicitHeight
    property bool _updatingLayout: false

    function polishTree(item: Item) {
        item.ensurePolished();
        for (const child of item.children)
            polishTree(child);
        item.ensurePolished();
    }

    function updateLayout(updateMarkdown: bool) {
        if (!col || _updatingLayout)
            return;
        _updatingLayout = true;
        if (updateMarkdown)
            markdownModel.setMarkdown(markdown);
        // Resolve wrapped text and nested layouts before publishing the content size.
        polishTree(col);
        root.DocumentScope.document?.restoreSelection();
        _updatingLayout = false;
    }

    onWidthChanged: updateLayout(false)

    MarkdownModel {
        id: markdownModel
        style: root.style
    }

    Column {
        id: col
        DocumentScope.style: root.style
        width: root.width
        spacing: 8

        Repeater {
            model: markdownModel

            MarkdownBlock {
                width: col.width
                required property int index
                blockIndex: index
                mdModel: markdownModel
                imageDelegate: root.imageDelegate
                fontFamily: root.fontFamily
            }
        }
    }

    onMarkdownChanged: updateLayout(true)
    Component.onCompleted: updateLayout(true)
}
