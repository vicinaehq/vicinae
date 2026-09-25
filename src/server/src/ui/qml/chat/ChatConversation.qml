pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property ChatSession session
    required property real horizontalPadding
    property real topInset: 0
    readonly property Item backdrop: transcript.backdrop
    readonly property real messageFontSize: Theme.regularFontSize + 1

    function focusComposer() {
        composer.forceActiveFocus();
    }
    function openFind() {
        findBar.open();
    }

    Keys.onShortcutOverride: event => event.accepted = findBar.visible && event.key === Qt.Key_Escape
    Keys.onEscapePressed: event => {
        event.accepted = findBar.visible;
        if (findBar.visible)
            findBar.close();
    }

    Connections {
        target: transcript.selection.search
        function onRevealRequested(row, part, position, length) {
            transcript.revealPosition(row, part, position, length);
        }
    }

    AttachmentPreview {
        id: preview
    }

    ChatTranscript {
        id: transcript
        anchors.fill: parent
        session: root.session
        compact: false
        fontSize: root.messageFontSize
        lineHeight: 1.2
        horizontalPadding: root.horizontalPadding
        typingTarget: composer.inputItem
        bottomPadding: 8
        topInset: root.topInset
        bottomInset: composerArea.height
        topOverlayHeight: findBar.visible ? findBar.height + 12 : 0
        onPreviewRequested: content => preview.show(content)
    }

    DocumentFindBar {
        id: findBar
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.right: parent.right
        anchors.rightMargin: Math.max(12, root.horizontalPadding)
        width: Math.min(implicitWidth, parent.width - 24)
        height: implicitHeight
        search: transcript.selection.search
        placeholder: qsTr("Find in conversation…")
        onClosed: composer.forceActiveFocus()
    }

    Column {
        anchors.centerIn: transcript
        anchors.verticalCenterOffset: -composerArea.height / 2
        width: Math.min(440, parent.width - 64)
        spacing: 16
        visible: transcript.count === 0

        ViciImage {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 64
            height: 64
            source: Img.local(":/icons/vicinae.png")
            opacity: 0.25
        }

        Text {
            width: parent.width
            text: qsTr("How can I help?")
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize + 8
            font.weight: Font.Normal
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }

        Text {
            width: parent.width
            visible: root.session.modelSelectorItems.length === 0
            text: qsTr("Add an AI provider to start a conversation.")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            lineHeight: 1.3
        }

        ViciButton {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: root.session.modelSelectorItems.length === 0
            text: qsTr("Set up AI")
            variant: "secondary"
            onClicked: Chat.openSettings()
        }
    }

    Item {
        id: composerArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: composer.height + 24

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.height + 8
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: Config.withAlpha(Theme.background, 0)
                }
                GradientStop {
                    position: 0.45
                    color: Config.withAlpha(Theme.background, 0.55)
                }
                GradientStop {
                    position: 1
                    color: Theme.background
                }
            }
        }

        ChatSessionComposer {
            id: composer
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - root.horizontalPadding * 2
            y: 8
            compact: false
            floating: true
            font.family: Theme.fontFamily
            font.pointSize: root.messageFontSize
            session: root.session
            placeholder: qsTr("Message…")
            onMessageSent: transcript.followLatest()
            onPreviewRequested: content => preview.show(content)

            onActiveFocusChanged: {
                if (activeFocus)
                    transcript.selection.clearSelection();
            }
        }
    }

    AttachmentDropArea {
        anchors.fill: parent
        attachmentModel: root.session.attachments
        enabled: !root.session.streaming
    }

    Component.onCompleted: composer.forceActiveFocus()
}
