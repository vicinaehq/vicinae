pragma ComponentBehavior: Bound
import QtQuick
import Vicinae
import Vicinae.Documents as Documents

ScrollViewport {
    id: root
    required property Documents.MarkdownModel model
    property int contentPadding: 12
    property string fontFamily: ""
    readonly property alias document: view.document
    readonly property alias contentHeight: view.contentHeight
    topPadding: contentPadding
    bottomPadding: contentPadding
    flickable: view.flickable

    function scrollUp() {
        view.scrollUp();
    }
    function scrollDown() {
        view.scrollDown();
    }

    Binding {
        target: root.model
        property: "style"
        value: DocumentIntegration.style
    }
    Documents.MarkdownView {
        id: view
        anchors.fill: parent
        model: root.model
        contentPadding: root.contentPadding
        topPadding: root.topPadding
        topInset: root.topInset
        bottomInset: root.bottomInset
        fontFamily: root.fontFamily
        imageDelegate: Component {
            VicinaeDocumentImage {}
        }
        onLinkActivated: link => DocumentIntegration.openLink(link)
    }
}
