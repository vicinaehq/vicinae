import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property string markdown: ""
    property int contentPadding: 12
    property string fontFamily: ""

    implicitHeight: view.contentHeight

    MarkdownModel {
        id: mdModel
    }

    MarkdownView {
        id: view
        anchors.fill: parent
        model: mdModel
        contentPadding: root.contentPadding
        fontFamily: root.fontFamily
    }

    onMarkdownChanged: mdModel.setMarkdown(markdown)
    Component.onCompleted: {
        if (markdown.length > 0)
            mdModel.setMarkdown(markdown)
    }
}
