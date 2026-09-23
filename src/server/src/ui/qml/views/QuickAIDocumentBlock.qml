pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property string kind
    required property string text
    required property int blockType
    required property var blockData
    required property int blockIndex
    required property var markdownModel
    required property bool pending
    required property var attachments
    required property bool failed
    required property var tool
    signal previewRequested(var content)
    signal toolToggled(var toolId)
    signal toolGroupToggled(var toolId)
    property int horizontalPadding: 16
    property int bottomSpacing: 8
    readonly property bool first: kind === "query"
    readonly property bool last: kind === "tail"
    readonly property bool groupedTool: kind === "tool" && (tool.grouped ?? false)
    readonly property real topPadding: first ? 12 : last && content.height === 0 ? 0 : groupedTool ? 2 : 8
    height: content.height + topPadding + (last ? 12 + bottomSpacing : 0)

    Item {
        x: root.horizontalPadding
        width: parent.width - root.horizontalPadding * 2
        height: parent.height - (root.last ? root.bottomSpacing : 0)
        clip: true
        Rectangle {
            width: parent.width
            y: root.first ? 0 : -8
            height: parent.height + (root.first ? 0 : 8) + (root.last ? 0 : 8)
            radius: 8
            color: root.failed ? Config.withAlpha(Theme.danger, 0.06) : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.04)
            border.color: root.failed ? Config.withAlpha(Theme.danger, 0.35) : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.12)
            border.width: 1
        }
    }
    Loader {
        id: content
        x: root.horizontalPadding + 12 + (root.groupedTool ? 20 : 0)
        y: root.topPadding
        width: parent.width - (root.horizontalPadding + 12) * 2 - (root.groupedTool ? 20 : 0)
        sourceComponent: root.first ? queryComponent : root.last ? tailComponent : root.kind === "tool" ? toolComponent : root.kind === "toolGroup" ? toolGroupComponent : markdownComponent
    }
    Component {
        id: queryComponent
        Column {
            spacing: 8
            DocumentText {
                width: parent.width
                visible: root.text.length > 0
                text: root.text
                color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
            }
            AttachmentList {
                width: parent.width
                attachments: root.attachments
                onPreviewRequested: content => root.previewRequested(content)
            }
        }
    }
    Component {
        id: toolGroupComponent
        ToolInvocationHeader {
            title: qsTr("%n tool calls", "", root.tool.count ?? 0)
            subtitle: root.tool.summary ?? ""
            status: root.tool.status ?? "queued"
            durationMs: root.tool.durationMs ?? null
            expanded: root.tool.expanded ?? false
            compactStatus: true
            onToggled: root.toolGroupToggled(root.tool.id)
        }
    }
    Component {
        id: toolComponent
        ToolInvocation {
            name: root.tool.name ?? ""
            summary: root.tool.summary ?? ""
            status: root.tool.status ?? "queued"
            statusText: root.tool.statusText ?? null
            durationMs: root.tool.durationMs ?? null
            expanded: root.tool.expanded ?? false
            arguments: root.tool.arguments ?? ""
            output: root.tool.output ?? ""
            hasOutput: root.tool.hasOutput ?? false
            typingTarget: root.DocumentScope.document?.typingTarget ?? null
            onToggled: root.toolToggled(root.tool.id)
        }
    }
    Component {
        id: markdownComponent
        MarkdownBlock {
            blockType: root.blockType
            blockData: root.blockData
            blockIndex: root.blockIndex
            mdModel: root.markdownModel
            fontFamily: Theme.fontFamily
        }
    }
    Component {
        id: tailComponent
        Item {
            implicitHeight: root.failed ? errorText.implicitHeight : root.pending ? 24 : 0
            PulsingDots {
                active: root.pending
                visible: active
                anchors.verticalCenter: parent.verticalCenter
            }
            DocumentText {
                id: errorText
                width: parent.width
                text: root.text
                visible: root.failed
                color: Theme.danger
            }
        }
    }
}
