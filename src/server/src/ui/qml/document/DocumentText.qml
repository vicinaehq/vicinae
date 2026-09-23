import QtQuick
import Vicinae

TextEdit {
    id: root
    property alias selectionPart: selection.part
    readOnly: true
    wrapMode: TextEdit.Wrap
    textFormat: TextEdit.PlainText
    color: Theme.foreground
    selectionColor: Theme.textSelectionBg
    selectedTextColor: Theme.textSelectionFg
    font.family: Theme.fontFamily
    font.pointSize: Theme.regularFontSize

    DocumentTextSelection {
        id: selection
    }
    DocumentTextImages {
        document: root.textDocument
        measuring: root.DocumentScope.measuring
    }
}
