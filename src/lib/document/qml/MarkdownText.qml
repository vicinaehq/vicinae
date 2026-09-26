pragma ComponentBehavior: Bound
import QtQuick
import Vicinae.Documents

Item {
    id: root
    property alias markdown: markdownModel.markdown
    property alias style: markdownModel.style
    property alias imageDelegate: view.imageDelegate
    property alias contentPadding: view.contentPadding
    property alias topPadding: view.topPadding
    property alias topInset: view.topInset
    property alias bottomInset: view.bottomInset
    property alias fontFamily: view.fontFamily
    readonly property alias flickable: view.flickable
    readonly property alias document: view.document
    readonly property alias contentHeight: view.contentHeight
    signal linkActivated(string link)
    implicitHeight: contentHeight

    function scrollUp() {
        view.scrollUp();
    }
    function scrollDown() {
        view.scrollDown();
    }

    MarkdownView {
        id: view
        anchors.fill: parent
        model: MarkdownModel {
            id: markdownModel
            style: DocumentStyle {}
        }
        onLinkActivated: link => root.linkActivated(link)
    }
}
