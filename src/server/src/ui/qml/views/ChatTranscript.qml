pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root
    required property ChatSession session
    property Item typingTarget: null
    property bool compact: true
    property real fontSize: Theme.regularFontSize
    property real lineHeight: 1.0
    property real horizontalPadding: compact ? 16 : 24
    property real bottomPadding: 8
    property real topInset: 0
    property real bottomInset: 0
    property real topOverlayHeight: 0
    readonly property Item backdrop: chatFlick
    readonly property int count: chatFlick.count
    signal previewRequested(var content)

    function revealPosition(row: int, part: int, position: int, length: int) {
        chatScroll.pauseFollowing();
        chatFlick.revealPosition(row, part, position, length);
    }

    function followLatest() {
        selection.clearSelection();
        chatScroll.scrollToBottom();
    }

    readonly property DocumentController selection: chatFlick.document

    Connections {
        target: chatFlick.document
        function onLinkActivated(link) {
            linkHandler.openLink(link);
        }
    }

    MarkdownModel {
        id: linkHandler
    }

    ScrollViewport {
        id: viewport
        anchors.fill: parent
        flickable: chatFlick
        topInset: root.topInset
        bottomInset: root.bottomInset
        topPadding: (root.compact ? 12 : 16) + root.topOverlayHeight
        bottomPadding: root.bottomPadding
        resetOnInitialization: false

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: Qt.IBeamCursor
        }

        ContextMenu.menu: DocumentSelectionMenu {
            controller: root.selection
            onAboutToShow: chatFlick.forceActiveFocus()
        }

        DocumentView {
            id: chatFlick
            documentModel: root.session.documentModel
            layoutKey: [Theme.fontFamily, Theme.monoFontFamily, Theme.regularFontSize, Theme.smallerFontSize, root.compact, root.horizontalPadding, root.fontSize, root.lineHeight]
            followEnd: chatScroll.following && !chatScroll.paused
            typingTarget: root.typingTarget
            topInset: viewport.topInset + root.topOverlayHeight
            bottomInset: viewport.bottomInset
            spacing: 0
            anchors.fill: parent

            Keys.onUpPressed: chatScroll.scrollBy(-40)
            Keys.onDownPressed: chatScroll.scrollBy(40)
            Keys.onPressed: event => {
                if (event.key === Qt.Key_PageUp)
                    chatScroll.scrollBy(-viewport.usableHeight);
                else if (event.key === Qt.Key_PageDown)
                    chatScroll.scrollBy(viewport.usableHeight);
                else if (event.key === Qt.Key_Home) {
                    chatScroll.pauseFollowing();
                    viewport.scrollTo(viewport.minimumY);
                } else if (event.key === Qt.Key_End)
                    chatScroll.scrollToBottom();
                else
                    event.accepted = false;
            }

            ViciWheelHandler {
                target: chatFlick
                onWheel: wheel => chatScroll.handleWheel(wheel)
            }

            ScrollBar.vertical: ViciScrollBar {
                id: chatScrollBar
                bottomPadding: viewport.bottomInset
            }

            delegate: ChatDocumentBlock {
                awaitingResponse: root.session.awaitingResponse
                thinking: root.session.thinking
                compact: root.compact
                fontSize: root.fontSize
                lineHeight: root.lineHeight
                horizontalPadding: root.horizontalPadding
                required property int index
                DocumentScope.row: index
                width: chatFlick.width
                onPreviewRequested: content => root.previewRequested(content)
                onToolGroupToggled: toolId => {
                    chatScroll.pauseFollowing();
                    root.session.toggleToolGroup(toolId);
                }
                onToolToggled: toolId => {
                    chatScroll.pauseFollowing();
                    root.session.toggleTool(toolId);
                }
            }

            footer: Row {
                x: 16
                height: visible ? implicitHeight : 0
                visible: root.compact && !root.session.streaming && root.session.modelLabel.length > 0
                spacing: 6

                ViciImage {
                    visible: root.session.modelIcon.valid
                    source: root.session.modelIcon
                    width: 20
                    height: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: root.session.modelLabel
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.35)
                    font.family: Theme.fontFamily
                    font.pointSize: Theme.smallerFontSize
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    ChatScrollController {
        id: chatScroll
        view: chatFlick
        scrollbar: chatScrollBar
    }

    ViciButton {
        id: jumpToLatest
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: viewport.bottomInset + 12
        visible: chatScroll.showJumpButton
        accessibleName: qsTr("Jump to latest")
        iconSource: Img.icon(BuiltinIcon.ArrowDown).withFillColor(Theme.foreground)
        variant: "primary"
        color: Config.withAlpha(Qt.tint(Theme.background, hovered || showFocus ? Theme.buttonPrimaryHoverBg : Theme.buttonPrimaryBg), 0.95)
        bordered: true
        radius: height / 2
        activeFocusOnTab: true
        ToolTip.visible: jumpToLatest.hovered
        ToolTip.text: jumpToLatest.accessibleName
        ToolTip.delay: 600
        onClicked: {
            root.selection.clearSelection();
            chatScroll.scrollToBottom();
            chatFlick.forceActiveFocus();
        }
    }
}
