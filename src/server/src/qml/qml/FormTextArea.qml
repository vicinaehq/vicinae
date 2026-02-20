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

    signal textEdited()

    function forceActiveFocus() { edit.forceActiveFocus() }
    function selectAll() { edit.selectAll() }

    onActiveFocusChanged: {
        if (activeFocus) edit.forceActiveFocus()
    }

    // Compute line height from font metrics
    readonly property real _lineHeight: Math.ceil(fontMetrics.height)
    readonly property real _verticalPadding: 8
    readonly property real _minHeight: _lineHeight * minRows + _verticalPadding * 2
    readonly property real _maxHeight: _lineHeight * maxRows + _verticalPadding * 2

    implicitHeight: Math.max(_minHeight, Math.min(flickable.contentHeight + _verticalPadding * 2, _maxHeight))

    FontMetrics {
        id: fontMetrics
        font: edit.font
    }

    Rectangle {
        id: border
        anchors.fill: parent
        radius: 8
        color: Theme.secondaryBackground
        border.color: edit.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
        border.width: 1

        Flickable {
            id: flickable
            anchors.fill: parent
            anchors.margins: root._verticalPadding
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            contentWidth: width
            contentHeight: edit.height
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            interactive: false

            ScrollBar.vertical: ScrollBar {
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

                Keys.onTabPressed: (event) => {
                    event.accepted = true
                    nextItemInFocusChain()?.forceActiveFocus(Qt.TabFocusReason)
                }
                Keys.onBacktabPressed: (event) => {
                    event.accepted = true
                    nextItemInFocusChain(false)?.forceActiveFocus(Qt.BacktabFocusReason)
                }
                Keys.onReturnPressed: (event) => {
                    if (event.modifiers !== Qt.NoModifier && typeof launcher !== "undefined") {
                        event.accepted = launcher.forwardKey(event.key, event.modifiers)
                    } else {
                        // Insert newline normally
                        edit.insert(edit.cursorPosition, "\n")
                    }
                }
                Keys.onPressed: (event) => {
                    if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
                        && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
                        && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta
                        && event.key !== Qt.Key_Tab && event.key !== Qt.Key_Backtab
                        && typeof launcher !== "undefined") {
                        event.accepted = launcher.forwardKey(event.key, event.modifiers)
                    }
                }
            }
        }

        // Scroll via mouse wheel instead of Flickable.interactive (which steals clicks)
        MouseArea {
            anchors.fill: flickable
            acceptedButtons: Qt.NoButton
            onWheel: (wheel) => {
                // Pass through to parent when there's nothing to scroll
                if (flickable.contentHeight <= flickable.height) {
                    wheel.accepted = false
                    return
                }
                flickable.contentY = Math.max(0,
                    Math.min(flickable.contentHeight - flickable.height,
                             flickable.contentY - wheel.angleDelta.y))
            }
        }
    }
}
