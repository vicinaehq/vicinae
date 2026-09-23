pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

DocumentText {
    id: root

    property var blockData: ({})
    property string fontFamily: ""

    width: parent?.width ?? 0
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    color: Theme.foreground
    font.pointSize: Theme.regularFontSize

    Binding on font.family {
        value: root.fontFamily
        when: root.fontFamily !== ""
    }

    text: blockData.html ?? ""
}
