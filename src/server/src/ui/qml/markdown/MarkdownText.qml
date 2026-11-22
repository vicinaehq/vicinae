pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property string markdown: ""
    property url baseUrl
    property int contentPadding: 12
    property int topPadding: contentPadding
    property string fontFamily: ""
    implicitHeight: view.contentHeight
    property alias contentHeight: view.contentHeight

    function scrollUp() {
        view.scrollUp();
    }
    function scrollDown() {
        view.scrollDown();
    }

    MarkdownModel {
        id: mdModel
        baseUrl: root.baseUrl
    }

    MarkdownView {
        id: view
        anchors.fill: parent
        model: mdModel
        contentPadding: root.contentPadding
        topPadding: root.topPadding
        fontFamily: root.fontFamily
    }

    onMarkdownChanged: mdModel.setMarkdown(markdown)
    Component.onCompleted: {
        if (markdown.length > 0)
            mdModel.setMarkdown(markdown);
    }
}
