pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Editor

Rectangle {
    id: root
    property color foreground: "#dddddd"
    property color backgroundColor: "#242428"
    property color borderColor: "#45454b"
    property color hoverColor: "#35353b"
    property var icons: ({})
    property bool headingsVisible: false
    signal formatRequested(int format)
    signal dismissed

    implicitWidth: inlineButtons.width + codeButtons.width + blockButtons.width + buttons.spacing * 2 + 12
    implicitHeight: buttons.height + 12 + (headingsVisible ? headingOptions.height + 10 : 0)
    radius: 9
    color: backgroundColor
    border.color: borderColor
    Accessible.role: Accessible.ToolBar
    Accessible.name: qsTr("Text formatting")

    Keys.onEscapePressed: root.dismissed()
    onVisibleChanged: if (!visible)
        headingsVisible = false

    component FormatButton: ToolButton {
        id: button
        required property string iconName
        required property string label
        required property string fallback
        implicitWidth: 30
        implicitHeight: 30
        padding: 2
        focusPolicy: Qt.TabFocus
        text: fallback
        display: icon.source.toString() !== "" ? AbstractButton.IconOnly : AbstractButton.TextOnly
        icon.source: root.icons[iconName] ?? ""
        icon.color: root.foreground
        icon.width: 16
        icon.height: 16
        palette.buttonText: root.foreground
        Accessible.name: label
        ToolTip.visible: hovered
        ToolTip.delay: 500
        ToolTip.text: label
        background: Rectangle {
            radius: 5
            color: button.hovered || button.down || button.checked || button.activeFocus ? root.hoverColor : "transparent"
            border.color: button.activeFocus ? root.foreground : "transparent"
        }
    }

    Flow {
        id: buttons
        x: 6
        y: 6
        width: parent.width - 12
        spacing: 12
        Row {
            id: inlineButtons
            spacing: 1
            FormatButton {
                iconName: "heading"
                label: qsTr("Heading")
                fallback: "H"
                checked: root.headingsVisible
                onClicked: root.headingsVisible = !root.headingsVisible
            }
            FormatButton {
                iconName: "bold"
                label: qsTr("Bold")
                fallback: "B"
                font.bold: true
                onClicked: root.formatRequested(MarkdownEditorDocument.Bold)
            }
            FormatButton {
                iconName: "italic"
                label: qsTr("Italic")
                fallback: "I"
                font.italic: true
                onClicked: root.formatRequested(MarkdownEditorDocument.Italic)
            }
            FormatButton {
                iconName: "strike"
                label: qsTr("Strikethrough")
                fallback: "S"
                font.strikeout: true
                onClicked: root.formatRequested(MarkdownEditorDocument.StrikeThrough)
            }
            FormatButton {
                iconName: "link"
                label: qsTr("Link")
                fallback: "↗"
                onClicked: root.formatRequested(MarkdownEditorDocument.Link)
            }
        }
        Row {
            id: codeButtons
            spacing: 1
            FormatButton {
                iconName: "code"
                label: qsTr("Inline Code")
                fallback: "<>"
                onClicked: root.formatRequested(MarkdownEditorDocument.InlineCode)
            }
            FormatButton {
                iconName: "codeBlock"
                label: qsTr("Code Block")
                fallback: "```"
                onClicked: root.formatRequested(MarkdownEditorDocument.CodeBlock)
            }
        }
        Row {
            id: blockButtons
            spacing: 1
            FormatButton {
                iconName: "bullets"
                label: qsTr("Bulleted List")
                fallback: "•"
                onClicked: root.formatRequested(MarkdownEditorDocument.BulletList)
            }
            FormatButton {
                iconName: "numbered"
                label: qsTr("Numbered List")
                fallback: "1."
                onClicked: root.formatRequested(MarkdownEditorDocument.NumberedList)
            }
            FormatButton {
                iconName: "tasks"
                label: qsTr("Task List")
                fallback: "☑"
                onClicked: root.formatRequested(MarkdownEditorDocument.TaskList)
            }
            FormatButton {
                iconName: "quote"
                label: qsTr("Quote")
                fallback: "❞"
                onClicked: root.formatRequested(MarkdownEditorDocument.Quote)
            }
        }
    }
    Rectangle {
        x: codeButtons.x - 1
        y: buttons.y + codeButtons.y + 6
        width: 1
        height: 18
        color: root.borderColor
        visible: codeButtons.y === inlineButtons.y
    }
    Rectangle {
        x: blockButtons.x - 1
        y: buttons.y + blockButtons.y + 6
        width: 1
        height: 18
        color: root.borderColor
        visible: blockButtons.y === codeButtons.y
    }
    Rectangle {
        x: 6
        y: buttons.y + buttons.height + 4
        width: parent.width - 12
        height: 1
        color: root.borderColor
        visible: root.headingsVisible
    }
    Row {
        id: headingOptions
        y: buttons.y + buttons.height + 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 1
        visible: root.headingsVisible
        Repeater {
            model: 6
            FormatButton {
                required property int index
                iconName: ""
                label: qsTr("Heading %1").arg(index + 1)
                fallback: "H" + (index + 1)
                font.bold: true
                onClicked: {
                    root.formatRequested(MarkdownEditorDocument.Heading1 + index);
                    root.headingsVisible = false;
                }
            }
        }
    }
}
