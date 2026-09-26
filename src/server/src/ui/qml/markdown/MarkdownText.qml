pragma ComponentBehavior: Bound
import QtQuick
import Vicinae
import Vicinae.Documents as Documents

MarkdownView {
    property alias markdown: markdownModel.markdown
    implicitHeight: contentHeight

    model: Documents.MarkdownModel {
        id: markdownModel
        style: DocumentIntegration.style
    }
}
