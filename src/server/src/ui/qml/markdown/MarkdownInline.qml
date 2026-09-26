import QtQuick
import Vicinae
import Vicinae.Documents as Documents

Documents.MarkdownInline {
    style: DocumentIntegration.style
    imageDelegate: Component {
        VicinaeDocumentImage {}
    }
}
