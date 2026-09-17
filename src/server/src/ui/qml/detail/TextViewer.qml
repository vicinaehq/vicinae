pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

ScrollView {
    id: root

    required property string text
    property bool monospace: false
    property list<string> highlightTerms
    property color highlightColor: Qt.alpha(Theme.accent, 0.35)

    clip: true
    contentWidth: availableWidth
    // required: the StatusBarInset child disables ScrollView's automatic content sizing
    contentHeight: textEdit.implicitHeight

    ScrollBar.vertical: ViciScrollBar {
        parent: root
        x: root.mirrored ? 0 : root.width - width
        y: root.topPadding
        height: root.availableHeight
    }

    Component.onCompleted: {
        contentItem.boundsBehavior = Flickable.StopAtBounds;
        if (searchBarInset.headerHeight > 0)
            Qt.callLater(root.scrollToFirstMatch);
    }

    onTextChanged: Qt.callLater(scrollToFirstMatch)

    function scrollToFirstMatch() {
        const flickable = root.contentItem;
        if (matchHighlighter.firstMatchPosition < 0) {
            flickable.contentY = -searchBarInset.value;
            return;
        }
        const rect = textEdit.positionToRectangle(matchHighlighter.firstMatchPosition);
        const target = rect.y - searchBarInset.value - (root.height - searchBarInset.value - rect.height) / 3;
        flickable.contentY = Math.max(-searchBarInset.value, Math.min(target, textEdit.height - root.height));
    }

    StatusBarInset {
        id: statusBarInset
        target: root
    }

    SearchBarInset {
        id: searchBarInset
        target: root
    }

    Binding {
        target: root.contentItem
        property: "topMargin"
        value: searchBarInset.value
    }

    Binding {
        target: root.contentItem
        property: "bottomMargin"
        value: statusBarInset.value
    }

    ViciWheelHandler {
        target: root.contentItem
    }

    TextEdit {
        id: textEdit
        width: root.availableWidth
        text: root.text
        textFormat: TextEdit.PlainText
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
        font.family: root.monospace ? Theme.monoFontFamily : Theme.fontFamily
        wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
        padding: 12
        readOnly: true
        selectByMouse: true
        selectionColor: Theme.textSelectionBg

        MatchHighlighter {
            id: matchHighlighter
            textDocument: textEdit.textDocument
            terms: root.highlightTerms
            color: root.highlightColor
            onFirstMatchPositionChanged: Qt.callLater(root.scrollToFirstMatch)
        }
    }
}
