pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

FocusScope {
    id: root
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: editor.text
    property alias textDocument: editor.textDocument
    property alias highlighter: editor.highlighter
    property alias placeholder: editor.placeholder
    property alias completions: editor.completions
    property alias triggerChar: editor.triggerChar
    property int minRows: 3
    property int maxRows: 10
    property bool hasError: false
    property bool filled: false

    signal textEdited

    function forceActiveFocus() {
        editor.forceActiveFocus();
    }
    function selectAll() {
        editor.selectAll();
    }

    onActiveFocusChanged: {
        if (activeFocus)
            editor.forceActiveFocus();
    }

    readonly property real _lineHeight: Math.ceil(fontMetrics.height)
    readonly property real _verticalPadding: 8
    readonly property real _minHeight: _lineHeight * minRows + _verticalPadding * 2
    readonly property real _maxHeight: _lineHeight * maxRows + _verticalPadding * 2

    implicitHeight: Math.max(_minHeight, Math.min(editor.contentHeight + _verticalPadding * 2, _maxHeight))

    FontMetrics {
        id: fontMetrics
        font: editor.font
    }

    FormInputFrame {
        anchors.fill: parent
        filled: root.filled
        hasError: root.hasError
        focused: editor.editing
    }

    FormTextEditor {
        id: editor
        anchors.fill: parent
        anchors.margins: root._verticalPadding
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        multiline: true

        onTextEdited: root.textEdited()
    }
}
