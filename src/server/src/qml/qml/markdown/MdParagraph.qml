import QtQuick
import Vicinae

TextEdit {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1

    width: parent?.width ?? 0
    readOnly: true
    selectByMouse: true
    selectionColor: Theme.textSelectionBg
    selectedTextColor: Theme.textSelectionFg
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: Theme.foreground
    font.pointSize: Theme.regularFontSize

    text: blockData.html ?? ""

    onLinkActivated: link => { if (mdModel) mdModel.openLink(link) }
}
