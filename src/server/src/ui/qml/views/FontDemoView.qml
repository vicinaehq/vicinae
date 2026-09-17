pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property FontDemoViewHost host

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
