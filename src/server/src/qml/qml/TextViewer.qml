import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    required property string text
    property bool monospace: false

    clip: true

    Text {
        width: root.availableWidth
        text: root.text
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
        font.family: root.monospace ? "monospace" : undefined
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        padding: 12
    }
}
