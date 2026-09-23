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
    required property bool failed
    property int horizontalPadding: 16
    property int bottomSpacing: 8
    readonly property bool first: kind === "query"
    readonly property bool last: kind === "tail"
    readonly property real topPadding: first ? 12 : last && content.height === 0 ? 0 : 8
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
        x: root.horizontalPadding + 12
        y: root.topPadding
        width: parent.width - (root.horizontalPadding + 12) * 2
        sourceComponent: root.first ? queryComponent : root.last ? tailComponent : markdownComponent
    }
    Component {
        id: queryComponent
        DocumentText {
            text: root.text
            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.5)
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
