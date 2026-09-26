pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae
import Vicinae.Scrolling as Scrolling

ScrollViewport {
    id: root
    flickable: scrollView.contentItem as Flickable
    verticalScrollBar: scrollView.ScrollBar.vertical

    required property string text
    property bool monospace: false
    property list<string> highlightTerms
    property color highlightColor: Qt.alpha(Theme.accent, 0.35)

    onTextChanged: Qt.callLater(scrollToFirstMatch)
    onInitialized: scrollToFirstMatch()

    function scrollToFirstMatch() {
        if (matchHighlighter.firstMatchPosition < 0) {
            root.resetPosition();
            return;
        }
        const rect = textEdit.positionToRectangle(matchHighlighter.firstMatchPosition);
        root.scrollTo(rect.y - root.topInset - (root.usableHeight - rect.height) / 3);
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        contentWidth: root.width
        contentHeight: textEdit.implicitHeight

        ScrollBar.vertical: ViciScrollBar {
            parent: scrollView
            x: scrollView.mirrored ? 0 : scrollView.width - width
            y: scrollView.topPadding
            height: scrollView.availableHeight
        }

        Component.onCompleted: contentItem.boundsBehavior = Flickable.StopAtBounds

        Scrolling.WheelHandler {
            target: root.flickable
        }

        TextEdit {
            id: textEdit
            width: root.width
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
}
