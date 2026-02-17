import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property string markdown: ""
    property int contentPadding: 12

    implicitHeight: view.contentHeight

    MarkdownModel {
        id: mdModel
    }

    MarkdownView {
        id: view
        anchors.fill: parent
        model: mdModel
        contentPadding: root.contentPadding
    }

    onMarkdownChanged: mdModel.setMarkdown(markdown)
    Component.onCompleted: {
        if (markdown.length > 0)
            mdModel.setMarkdown(markdown)
    }
}
