import QtQuick
import Vicinae

TextEdit {
    id: root
    property alias selectionPart: selection.part
    property real fontSize: Theme.regularFontSize
    property real lineHeight: 1.0
    readOnly: true
    wrapMode: TextEdit.Wrap
    textFormat: TextEdit.PlainText
    color: Theme.foreground
    selectionColor: Theme.textSelectionBg
    selectedTextColor: Theme.textSelectionFg
    font.family: Theme.fontFamily
    font.pointSize: fontSize

    onTextChanged: if (lineHeight !== 1.0)
        TextDocumentEdit.setLineHeight(textDocument, lineHeight)
    onLineHeightChanged: TextDocumentEdit.setLineHeight(textDocument, lineHeight)

    DocumentTextSelection {
        id: selection
    }
    DocumentTextImages {
        document: root.textDocument
        measuring: root.DocumentScope.measuring
    }
}
