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

            ViciButton {
                Layout.rightMargin: 12
                iconSource: Img.icon(BuiltinIcon.ArrowsExpand).withFillColor(Theme.textMuted)
                accessibleName: qsTr("Open in AI Chat")
                activeFocusOnTab: true
                ToolTip.visible: hovered
                ToolTip.text: accessibleName
                ToolTip.delay: 600
                onClicked: root.host.openInWindow()
            }
        }
    }

    AttachmentPreview {
        id: attachmentPreview
    }

    ChatTranscript {
        id: transcript
        anchors.fill: parent
        session: root.host.session
        typingTarget: composer.inputItem
        topInset: root.topInset
        bottomInset: root.bottomInset
        bottomPadding: Math.max(0, 8 - root.appearance.contentBottomInset)
        onPreviewRequested: content => attachmentPreview.show(content)
    }

    EmptyView {
        width: parent.width
        height: parent.height
        visible: transcript.count === 0 && !root.host.session.streaming
        icon: Launcher.navigationIcon
        title: qsTr("Ask anything")
        description: qsTr("Answers use the model selected in the composer.")
    }

    AttachmentDropArea {
        anchors.fill: parent
        attachmentModel: root.host.session.attachments
        enabled: !root.host.session.streaming
        z: 1
    }

    footer: Item {
        implicitHeight: composer.height + 10
        height: implicitHeight

        ChatSessionComposer {
            id: composer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            session: root.host.session
            placeholder: transcript.count === 0 && !root.host.session.streaming ? qsTr("Ask anything...") : qsTr("Ask a follow-up...")
            onMessageSent: transcript.followLatest()
            onPreviewRequested: content => attachmentPreview.show(content)

            onActiveFocusChanged: {
                if (activeFocus)
                    transcript.selection.clearSelection();
            }
        }
    }

    Component.onCompleted: composer.forceActiveFocus()
}
