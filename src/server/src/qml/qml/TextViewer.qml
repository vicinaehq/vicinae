import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    required property string text
    property bool monospace: false

    clip: true
    contentWidth: availableWidth

    TextEdit {
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
    }
}
