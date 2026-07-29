import QtQuick
import Vicinae

Item {
    id: root
    required property var host

    function moveUp() {
        mdContent.scrollUp();
        return true;
    }
    function moveDown() {
        mdContent.scrollDown();
        return true;
    }

    MarkdownText {
        id: mdContent
        anchors.fill: parent
        markdown: root.host.showcaseMarkdown
        fontFamily: root.host.fontFamily
    }
}
