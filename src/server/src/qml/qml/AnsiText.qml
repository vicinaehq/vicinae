import QtQuick

Text {
    id: root

    property string rawText: ""

    readonly property bool _hasAnsi: rawText.indexOf('\x1b') !== -1

    textFormat: _hasAnsi ? Text.RichText : Text.PlainText
    text: _hasAnsi ? Ansi.toHtml(rawText) : rawText
}
