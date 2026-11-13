pragma ComponentBehavior: Bound
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
        devicePixelRatio: root.Screen.devicePixelRatio
    }
    DocumentSearchHighlighter {
        id: searchHighlighter
        search: root.DocumentScope.document?.search ?? null
        row: root.DocumentScope.row
        part: root.selectionPart
        textDocument: search && search.query.length > 0 ? root.textDocument : null
    }
    Repeater {
        model: searchHighlighter.rects
        delegate: Rectangle {
            id: highlight
            required property var modelData
            z: -1
            x: root.leftPadding + highlight.modelData.rect.x
            y: root.topPadding + highlight.modelData.rect.y
            width: highlight.modelData.rect.width
            height: highlight.modelData.rect.height
            radius: 2
            color: Config.withAlpha(Theme.accent, highlight.modelData.current ? 0.55 : 0.25)
        }
    }
}
