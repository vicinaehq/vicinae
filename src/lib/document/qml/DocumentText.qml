import QtQuick
import Vicinae.Documents

TextEdit {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style
    property alias selectionPart: selection.part
    property real fontSize: root.style.regularFontSize
    property real lineHeight: 1.0
    readOnly: true
    wrapMode: TextEdit.Wrap
    textFormat: TextEdit.PlainText
    color: root.style.foreground
    selectionColor: root.style.textSelectionBg
    selectedTextColor: root.style.textSelectionFg
    font.family: root.style.fontFamily
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
        devicePixelRatio: root.Screen.devicePixelRatio
    }
    DocumentSearchHighlighter {
        anchors.fill: parent
        z: -1
        textItem: root
        search: root.DocumentScope.document?.search ?? null
        row: root.DocumentScope.row
        part: root.selectionPart
        textDocument: search && search.query.length > 0 ? root.textDocument : null
        color: Qt.rgba(root.style.accent.r, root.style.accent.g, root.style.accent.b, 0.25)
        currentColor: Qt.rgba(root.style.accent.r, root.style.accent.g, root.style.accent.b, 0.55)
    }
}
