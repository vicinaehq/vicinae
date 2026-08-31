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
    // StatusBarInset makes this a multi-child ScrollView, which disables automatic
    // content sizing from the single child's implicit height
    contentHeight: textEdit.implicitHeight

    ScrollBar.vertical: ViciScrollBar {
        parent: root
        x: root.mirrored ? 0 : root.width - width
        y: root.topPadding
        height: root.availableHeight
    }

    Component.onCompleted: contentItem.boundsBehavior = Flickable.StopAtBounds

    onTextChanged: Qt.callLater(scrollToFirstMatch)

    function scrollToFirstMatch() {
        const flickable = root.contentItem;
        if (matchHighlighter.firstMatchPosition < 0) {
            flickable.contentY = 0;
            return;
        }
        const rect = textEdit.positionToRectangle(matchHighlighter.firstMatchPosition);
        const target = rect.y - (root.height - rect.height) / 3;
        flickable.contentY = Math.max(0, Math.min(target, textEdit.height - root.height));
    }

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

    TextEdit {
        id: textEdit
        width: root.availableWidth
        text: root.text
        textFormat: TextEdit.PlainText
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
        font.family: root.monospace ? Theme.monoFontFamily : undefined
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
