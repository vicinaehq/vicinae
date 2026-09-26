pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Documents

ApplicationWindow {
    id: window
    width: 960
    height: 780
    minimumWidth: 480
    minimumHeight: 400
    visible: true
    title: qsTr("QML Document — declarative composition")
    color: palette.base

    Document {
        id: page
        anchors.fill: parent
        padding: 28
        spacing: 20
        style: DocumentStyle {
            regularFontSize: 14
        }
        onLinkActivated: link => status.text = qsTr("The host received: %1").arg(link)

        DocumentText {
            width: parent.width
            fontSize: 26
            font.weight: Font.Bold
            text: qsTr("A document written in QML")
        }
        DocumentText {
            width: parent.width
            text: qsTr("Drag a selection from this paragraph through the Markdown and into the cards below. Everything belongs to one document, without a custom model.")
        }
        MarkdownInline {
            width: parent.width
            markdown: qsTr("## A little Markdown\n\nA **formatted paragraph**, some `inline code`, and an equation: $x \\in \\mathbb{R}$. These blocks inherit the surrounding document’s style.\n\n- Selection crosses component boundaries.\n- Copy and Select All work across the whole document.")
        }
        BuildCard {
            width: parent.width
            title: qsTr("Document library")
            description: qsTr("This card is a reusable QML component. Its text participates through DocumentText; its progress widget supplies a custom copy representation.")
            progress: 1
        }
        BuildCard {
            width: parent.width
            title: qsTr("Example application")
            description: qsTr("Select the progress bar to copy its meaning as text. The same component can be used anywhere inside a document.")
            progress: 0.75
        }
        CheckBox {
            id: details
            text: qsTr("Show an extra paragraph")
        }
        DocumentText {
            width: parent.width
            visible: details.checked
            text: qsTr("Ordinary controls remain interactive. This paragraph joins the document when shown and leaves it when hidden.")
        }
        MarkdownInline {
            width: parent.width
            markdown: qsTr("## One shared surface\n\n| Component | Selection |\n| --- | --- |\n| DocumentText | Native text ranges |\n| MarkdownInline | Across rendered blocks |\n| BuildCard | Text and a custom widget |\n\n[Try a link](example://details) to see the action forwarded to the host.\n\nAll components remain instantiated. This mode is intended for smaller documents.")
        }
    }

    footer: ToolBar {
        height: 36
        Label {
            id: status
            anchors.fill: parent
            anchors.leftMargin: 20
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: qsTr("QML composition · Shared selection · No custom model")
        }
    }
}
