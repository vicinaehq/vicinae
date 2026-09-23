pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

LauncherView {
    id: root
    required property QuickAIViewHost host

    function restoreFocus() {
        composer.forceActiveFocus();
    }

    header: Item {
        implicitHeight: root.appearance.searchBarHeight
        height: implicitHeight

        RowLayout {
            anchors.fill: parent
            spacing: 12

            ViciImage {
                Layout.leftMargin: 16
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                source: Img.icon(BuiltinIcon.ChevronLeft).withFillColor(Theme.textMuted)
                opacity: backHover.hovered ? 0.6 : 1.0

                HoverHandler {
                    id: backHover
                    cursorShape: Qt.PointingHandCursor
                }

                TapHandler {
                    onTapped: Launcher.goBack()
                }
            }

            Text {
                text: Launcher.navigationTitle
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.regularFontSize
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
                Layout.rightMargin: 16
            }
        }
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

    AttachmentPreview {
        id: attachmentPreview
    }

    ScrollViewport {
        id: viewport
        anchors.fill: parent
        flickable: chatFlick
        topPadding: 12
        bottomPadding: Math.max(0, 8 - root.appearance.contentBottomInset)
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
            documentModel: root.host.documentModel
            followEnd: chatScroll.following && !chatScroll.paused
            typingTarget: composer.inputItem
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
            }

            delegate: QuickAIDocumentBlock {
                required property int index
                DocumentScope.row: index
                width: chatFlick.width
                onPreviewRequested: content => attachmentPreview.show(content)
                onToolGroupToggled: toolId => {
                    chatScroll.pauseFollowing();
                    root.host.toggleToolGroup(toolId);
                }
                onToolToggled: toolId => {
                    chatScroll.pauseFollowing();
                    root.host.toggleTool(toolId);
                }
            }

            footer: Row {
                x: 16
                visible: !root.host.streaming && root.host.modelLabel.length > 0
                spacing: 6

                ViciImage {
                    visible: root.host.modelIcon.valid
                    source: root.host.modelIcon
                    width: 20
                    height: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: root.host.modelLabel
                    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.35)
                    font.family: Theme.fontFamily
                    font.pointSize: Theme.smallerFontSize
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    QuickAIChatScroll {
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

    EmptyView {
        width: viewport.width
        height: viewport.height - viewport.bottomInset
        visible: chatFlick.count === 0 && !root.host.streaming
        icon: Launcher.navigationIcon
        title: qsTr("Ask anything")
        description: qsTr("Answers use the model selected in the composer.")
    }

    AttachmentDropArea {
        anchors.fill: parent
        attachmentModel: root.host.attachments
        enabled: !root.host.streaming
        z: 1
    }

    footer: Item {
        implicitHeight: composer.height + 10
        height: implicitHeight

        ChatComposer {
            id: composer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            placeholder: chatFlick.count === 0 && !root.host.streaming ? qsTr("Ask anything...") : qsTr("Ask a follow-up...")
            busy: root.host.streaming
            attachments: root.host.attachments
            submissionEnabled: root.host.canSend
            message: root.host.attachmentMessage
            modelItems: root.host.modelSelectorItems
            currentModel: root.host.modelSelectorCurrentItem
            dictationAvailable: root.host.dictationAvailable
            recording: root.host.recording
            transcribing: root.host.transcribing
            recordingTime: root.host.recordingTime
            dictationMessage: root.host.dictationMessage
            onSubmitted: text => {
                if (root.host.send(text)) {
                    composer.text = "";
                    root.selection.clearSelection();
                    chatScroll.following = true;
                }
            }
            onCancelled: root.host.cancel()
            onPreviewRequested: content => attachmentPreview.show(content)
            onModelActivated: item => root.host.selectModel(item.id)
            onDictationToggled: root.host.toggleDictation()
            onDictationCancelled: root.host.cancelDictation()

            onActiveFocusChanged: {
                if (activeFocus)
                    root.selection.clearSelection();
            }
        }
    }

    Component.onCompleted: composer.forceActiveFocus()

    Connections {
        target: root.host
        function onDictated(text) {
            composer.insertText(text);
        }
    }
}
