pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

ScrollView {
    id: root

    property alias text: textArea.text
    readonly property Flickable flick: contentItem as Flickable

    // required: the StatusBarInset child disables ScrollView's automatic content sizing
    contentWidth: availableWidth
    contentHeight: textArea.implicitHeight

    ScrollBar.vertical: ViciScrollBar {
        parent: root
        x: root.mirrored ? 0 : root.width - width
        y: root.topPadding
        height: root.availableHeight
    }

    Component.onCompleted: contentItem.boundsBehavior = Flickable.StopAtBounds

    StatusBarInset {
        id: statusBarInset
        target: root
    }

    Binding {
        target: root.contentItem
        property: "bottomMargin"
        value: statusBarInset.value
    }

    ViciWheelHandler {
        target: root.contentItem
    }

    function moveUp() {
        root.flick.contentY = Math.max(0, root.flick.contentY - 40);
        return true;
    }
    function moveDown() {
        root.flick.contentY = Math.min(root.flick.contentHeight - height + root.flick.bottomMargin, root.flick.contentY + 40);
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
        root.flick.contentY = Math.max(0, root.flick.contentHeight - root.height + root.flick.bottomMargin);
    }

    TextArea {
        id: textArea
        width: root.availableWidth
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
