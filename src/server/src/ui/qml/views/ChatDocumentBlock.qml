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
    property bool awaitingResponse: false
    property bool thinking: false
    signal previewRequested(var content)
    signal toolToggled(var toolId)
    signal toolGroupToggled(var toolId)
    property bool compact: true
    property real fontSize: Theme.regularFontSize
    property real lineHeight: 1.0
    property real horizontalPadding: 16
    property int bottomSpacing: compact ? 8 : 24
    readonly property bool first: kind === "query"
    readonly property bool last: kind === "tail"
    readonly property bool groupedTool: kind === "tool" && (tool.grouped ?? false)
    readonly property real topPadding: first ? (compact ? 12 : 0) : last && content.height === 0 ? 0 : groupedTool ? 2 : 8
    height: content.height + topPadding + (last ? (compact ? 12 : 0) + bottomSpacing : 0)

    Item {
        visible: root.compact
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
        x: root.horizontalPadding + (root.compact ? 12 : 0) + (root.groupedTool ? 20 : 0)
        y: root.topPadding
        width: parent.width - (root.horizontalPadding + (root.compact ? 12 : 0)) * 2 - (root.groupedTool ? 20 : 0)
        sourceComponent: root.first ? (root.compact ? queryComponent : messageComponent) : root.last ? tailComponent : root.kind === "tool" ? toolComponent : root.kind === "toolGroup" ? toolGroupComponent : markdownComponent
    }
    Component {
        id: queryComponent
        Column {
            spacing: 8
            DocumentText {
                fontSize: root.fontSize
                lineHeight: root.lineHeight
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
        id: messageComponent
        Item {
            height: userMessage.height + 12

            Rectangle {
                id: userMessage
                anchors.right: parent.right
                width: Math.min(parent.width * 0.88, Math.max(48, queryMetrics.advanceWidth + 28, root.attachments.length > 0 ? 320 : 0))
                height: queryContent.height + 20
                radius: 14
                color: Config.withAlpha(Theme.foreground, 0.065)

                TextMetrics {
                    id: queryMetrics
                    font.family: Theme.fontFamily
                    font.pointSize: root.fontSize
                    text: root.text
                }

                Column {
                    id: queryContent
                    x: 14
                    y: 10
                    width: parent.width - 28
                    spacing: 8
                    DocumentText {
                        fontSize: root.fontSize
                        lineHeight: root.lineHeight
                        width: parent.width
                        visible: root.text.length > 0
                        text: root.text
                    }
                    AttachmentList {
                        width: parent.width
                        attachments: root.attachments
                        onPreviewRequested: content => root.previewRequested(content)
                    }
                }
            }
        }
    }
    Component {
        id: toolGroupComponent
        ToolInvocationHeader {
            title: qsTr("%n tool calls", "", root.tool.count ?? 0)
            subtitle: root.tool.summary ? qsTr("%1 · %2").arg(root.tool.name ?? "").arg(root.tool.summary) : root.tool.name ?? ""
            iconSource: root.tool.iconSource ?? ""
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
            iconSource: root.tool.iconSource ?? ""
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
            fontSize: root.fontSize
            lineHeight: root.lineHeight
            maxImageHeight: root.compact ? 200 : 360
        }
    }
    Component {
        id: tailComponent
        Item {
            id: tailContent
            readonly property bool showActivity: root.pending && root.awaitingResponse
            implicitHeight: root.failed ? errorText.implicitHeight : showActivity ? 24 : 0
            Row {
                width: parent.width
                height: parent.height
                visible: tailContent.showActivity && !root.failed
                spacing: 8

                Text {
                    width: Math.min(implicitWidth, Math.max(0, parent.width - activityDots.width - parent.spacing))
                    anchors.verticalCenter: parent.verticalCenter
                    visible: root.thinking
                    text: qsTr("Thinking")
                    textFormat: Text.PlainText
                    elide: Text.ElideRight
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pointSize: root.fontSize - 1
                    Accessible.role: Accessible.StaticText
                    Accessible.name: text
                }

                PulsingDots {
                    id: activityDots
                    anchors.verticalCenter: parent.verticalCenter
                    active: visible
                }
            }
            DocumentText {
                id: errorText
                fontSize: root.fontSize
                lineHeight: root.lineHeight
                width: parent.width
                text: root.text
                visible: root.failed
                color: Theme.danger
            }
        }
    }
}
