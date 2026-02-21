import QtQuick
import Vicinae

TextEdit {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property string fontFamily: ""

    readonly property var scaleFactors: [2.0, 1.6, 1.3, 1.16, 1.0]
    readonly property int level: Math.max(1, Math.min(5, blockData.level ?? 1))

    width: parent?.width ?? 0
    readOnly: true
    selectionColor: Theme.textSelectionBg
    selectedTextColor: Theme.textSelectionFg
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: Theme.foreground

    font.pointSize: Theme.regularFontSize * scaleFactors[level - 1]

    Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }
    font.bold: true

    topPadding: blockIndex === 0 ? 0 : (level <= 2 ? 12 : 8)
    bottomPadding: level <= 2 ? 8 : 4

    text: blockData.html ?? ""

    onSelectionControllerChanged: if (selectionController) selectionController.registerSelectable(root, blockIndex * 10000, true)
    Component.onDestruction: if (selectionController) selectionController.unregisterSelectable(root)
}
