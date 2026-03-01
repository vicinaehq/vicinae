import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    Layout.fillWidth: true
    activeFocusOnTab: true

    property alias text: edit.text
    property string placeholder: ""
    property int minRows: 3
    property int maxRows: 10

    property bool hasError: false

    signal textEdited()

    function forceActiveFocus() { edit.forceActiveFocus() }
    function selectAll() { edit.selectAll() }

    onActiveFocusChanged: {
        if (activeFocus) edit.forceActiveFocus()
    }

    readonly property real _lineHeight: Math.ceil(fontMetrics.height)
    readonly property real _verticalPadding: 8
    readonly property real _minHeight: _lineHeight * minRows + _verticalPadding * 2
    readonly property real _maxHeight: _lineHeight * maxRows + _verticalPadding * 2

    implicitHeight: Math.max(_minHeight, Math.min(edit.contentHeight + _verticalPadding * 2, _maxHeight))

    FontMetrics {
        id: fontMetrics
        font: edit.font
    }

    Rectangle {
        id: border
        anchors.fill: parent
        radius: 8
        color: Theme.secondaryBackground
        border.color: root.hasError ? Theme.inputBorderError
                      : edit.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
        border.width: 1

        MouseArea {
            anchors.fill: parent
            anchors.margins: root._verticalPadding
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            acceptedButtons: Qt.NoButton
            onWheel: (wheel) => {}

            Flickable {
                id: flickable
                anchors.fill: parent
                contentWidth: width
                contentHeight: edit.height
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                ScrollBar.vertical: ViciScrollBar {
                    policy: flickable.contentHeight > flickable.height
                            ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                }

                TextEdit {
                    id: edit
                    width: flickable.width
                    height: Math.max(contentHeight, flickable.height)
                    font.pointSize: Theme.regularFontSize
                    color: Theme.foreground
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    wrapMode: TextEdit.Wrap
                    activeFocusOnTab: false

                    Text {
                        anchors.top: parent.top
                        width: parent.width
                        text: root.placeholder
                        color: Theme.textPlaceholder
                        font: edit.font
                        wrapMode: Text.Wrap
                        visible: !edit.text && !edit.preeditText
                    }

                    onTextChanged: root.textEdited()

                    onCursorRectangleChanged: {
                        const rect = cursorRectangle
                        if (rect.y < flickable.contentY)
                            flickable.contentY = rect.y
                        else if (rect.y + rect.height > flickable.contentY + flickable.height)
                            flickable.contentY = rect.y + rect.height - flickable.height
                    }

                    Keys.onTabPressed: (event) => {
                        event.accepted = true
                        nextItemInFocusChain()?.forceActiveFocus(Qt.TabFocusReason)
                    }
                    Keys.onBacktabPressed: (event) => {
                        event.accepted = true
                        nextItemInFocusChain(false)?.forceActiveFocus(Qt.BacktabFocusReason)
                    }
                    Keys.onReturnPressed: (event) => {
                        if (typeof launcher !== "undefined")
                            event.accepted = launcher.forwardKey(event.key, event.modifiers)
                        if (!event.accepted)
                            edit.insert(edit.cursorPosition, "\n")
                    }
                    Keys.onPressed: (event) => {
                        if (typeof launcher !== "undefined")
                            event.accepted = launcher.forwardKey(event.key, event.modifiers)
                    }
                }
            }
        }
    }
}
