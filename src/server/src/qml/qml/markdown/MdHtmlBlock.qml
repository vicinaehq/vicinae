import QtQuick
import Vicinae

TextEdit {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property string fontFamily: ""

    width: parent?.width ?? 0
    readOnly: true
    selectionColor: Theme.textSelectionBg
    selectedTextColor: Theme.textSelectionFg
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: Theme.foreground
    font.pointSize: Theme.regularFontSize

    Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }

    text: blockData.html ?? ""

    onSelectionControllerChanged: if (selectionController) selectionController.registerSelectable(root, blockIndex * 10000, true)
    Component.onDestruction: if (selectionController) selectionController.unregisterSelectable(root)
}
