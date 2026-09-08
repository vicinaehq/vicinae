pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property StoreIntroViewHost host

    MarkdownText {
        anchors.fill: parent
        markdown: root.host.introMarkdown
        contentPadding: 20
    }
}
