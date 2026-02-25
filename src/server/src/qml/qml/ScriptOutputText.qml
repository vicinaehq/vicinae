import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    property alias text: textArea.text

    function moveUp() {
        contentItem.contentY = Math.max(0, contentItem.contentY - 40)
    }
    function moveDown() {
        contentItem.contentY = Math.min(
            contentItem.contentHeight - height,
            contentItem.contentY + 40
        )
    }
    function focusText() { textArea.forceActiveFocus() }
    function scrollToBottom() {
        let flick = contentItem
        flick.contentY = Math.max(0, flick.contentHeight - root.height)
    }

    TextArea {
        id: textArea
        textFormat: TextArea.RichText
        color: Theme.foreground
        readOnly: true
        selectByMouse: true
        selectionColor: Theme.accent
        selectedTextColor: Theme.foreground
        wrapMode: TextArea.WrapAtWordBoundaryOrAnywhere
        background: null
        topPadding: 12
        bottomPadding: 12
        leftPadding: 15
        rightPadding: 15
        onLinkActivated: (link) => Qt.openUrlExternally(link)
    }
}
