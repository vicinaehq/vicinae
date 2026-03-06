import QtQuick
import "markdown"

MarkdownText {
    anchors.fill: parent
    markdown: host.detailContent
    contentPadding: 16
}
