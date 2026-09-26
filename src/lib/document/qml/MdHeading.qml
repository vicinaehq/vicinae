pragma ComponentBehavior: Bound
import QtQuick
import Vicinae.Documents

DocumentText {
    id: root

    property var blockData: ({})
    property string fontFamily: ""
    property bool first: false

    readonly property var scaleFactors: [2.0, 1.6, 1.3, 1.16, 1.0]
    readonly property int level: Math.max(1, Math.min(5, blockData.level ?? 1))

    width: parent?.width ?? 0
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: root.style.foreground

    font.pointSize: fontSize * scaleFactors[level - 1]

    Binding on font.family {
        value: root.fontFamily
        when: root.fontFamily !== ""
    }
    font.bold: true

    topPadding: first ? 0 : (level <= 2 ? 12 : 8)
    bottomPadding: level <= 2 ? 8 : 4

    text: blockData.html ?? ""
}
