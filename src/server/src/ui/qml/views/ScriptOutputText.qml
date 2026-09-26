pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae
import Vicinae.Scrolling as Scrolling

ScrollViewport {
    id: root
    flickable: scrollView.contentItem as Flickable
    verticalScrollBar: scrollView.ScrollBar.vertical

    property alias text: textArea.text
    readonly property alias flick: root.flickable
    readonly property real contentHeight: flick.contentHeight

    function moveUp() {
        root.scrollTo(root.flick.contentY - 40);
        return true;
    }
    function moveDown() {
        root.scrollTo(root.flick.contentY + 40);
        return true;
    }
    function moveSectionUp() {
        return moveUp();
    }
    function moveSectionDown() {
        return moveDown();
    }
    function focusText() {
        textArea.forceActiveFocus();
    }
    function scrollToBottom() {
        root.scrollTo(root.maximumY);
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentWidth: root.width - leftPadding - rightPadding
        contentHeight: textArea.implicitHeight

        ScrollBar.vertical: ViciScrollBar {
            parent: scrollView
            x: scrollView.mirrored ? 0 : scrollView.width - width
            y: scrollView.topPadding
            height: scrollView.availableHeight
        }

        Component.onCompleted: contentItem.boundsBehavior = Flickable.StopAtBounds

        Scrolling.WheelHandler {
            target: root.flick
        }

        TextArea {
            id: textArea
            width: scrollView.availableWidth
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
            onLinkActivated: link => Qt.openUrlExternally(link)
        }
    }
}
