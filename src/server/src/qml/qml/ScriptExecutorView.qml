import QtQuick
import QtQuick.Controls

Flickable {
    id: root
    required property var host

    function moveUp() {
        contentY = Math.max(0, contentY - 40)
    }
    function moveDown() {
        contentY = Math.min(contentHeight - height, contentY + 40)
    }

    clip: true
    contentWidth: width
    contentHeight: outputText.implicitHeight
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.vertical: ViciScrollBar { policy: ScrollBar.AsNeeded }

    onContentHeightChanged: {
        // Auto-scroll to bottom when new output arrives
        if (contentY >= contentHeight - height - 60) {
            contentY = Math.max(0, contentHeight - height)
        }
    }

    Text {
        id: outputText
        width: root.width
        text: root.host.outputHtml
        textFormat: Text.RichText
        color: Theme.foreground
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        onLinkActivated: (link) => Qt.openUrlExternally(link)
    }
}
