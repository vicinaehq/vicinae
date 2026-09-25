pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Window {
    id: root
    property bool nativeChrome: false
    property bool translucentSidebar: nativeChrome || Platform.supports("windowMaterial")
    property bool sidebarVisible: true
    readonly property int sidebarWidth: sidebarVisible ? 240 : 0
    readonly property list<Item> titlebarControls: [sidebar.titlebarControls, showSidebarButton, titleEditor, newChatButton, moreButton]
    readonly property int headerHeight: 44
    readonly property real contentPadding: Math.max(24, (width - sidebarWidth - 800) / 2)
    width: 1120
    height: 780
    minimumWidth: 720
    minimumHeight: 520
    color: "transparent"
    title: Chat.session?.conversationId ? qsTr("%1 — Vicinae").arg(Chat.session.title) : qsTr("AI Chat — Vicinae")
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint
    WindowMaterial.enabled: Platform.supports("windowMaterial")

    Shortcut {
        sequences: [StandardKey.Close]
        onActivated: root.close()
    }
    Shortcut {
        sequences: [StandardKey.New]
        onActivated: Chat.newChat()
    }
    Shortcut {
        sequences: [StandardKey.Find]
        onActivated: (conversation.item as ChatConversation)?.openFind()
    }
    Shortcut {
        sequence: "Ctrl+Shift+F"
        onActivated: Chat.searchConversations(sidebar.searchButton)
    }

    Rectangle {
        anchors.fill: parent
        color: root.translucentSidebar ? "transparent" : Theme.background
    }

    Rectangle {
        width: root.sidebarWidth
        height: parent.height
        visible: root.sidebarVisible
        color: root.translucentSidebar ? Config.withAlpha(Theme.background, Qt.platform.os === "linux" ? 0.96 : 0.72) : Config.withAlpha(Theme.foreground, 0.025)
    }

    ChatSidebar {
        id: sidebar
        width: root.sidebarWidth
        height: parent.height
        visible: root.sidebarVisible
        nativeChrome: root.nativeChrome
        headerHeight: root.headerHeight
        onHideRequested: root.sidebarVisible = false
        onMoveRequested: root.startSystemMove()
        onDeleteRequested: (conversationId, title) => root.confirmDelete(conversationId, title)
    }

    Rectangle {
        x: root.sidebarWidth
        width: parent.width - x
        height: parent.height
        color: Theme.background

        Item {
            id: toolbar
            z: 1
            width: parent.width
            height: root.headerHeight

            BackdropSurface {
                anchors.fill: parent
                sourceItem: (conversation.item as ChatConversation)?.backdrop ?? null
                color: Qt.tint(Theme.background, Config.withAlpha(Theme.foreground, 0.025))
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Config.withAlpha(Theme.foreground, 0.09)
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onWheel: wheel => wheel.accepted = false
            }

            DragHandler {
                enabled: root.nativeChrome && !titleEditor.editing
                target: null
                grabPermissions: PointerHandler.CanTakeOverFromHandlersOfSameType | PointerHandler.CanTakeOverFromHandlersOfDifferentType
                onActiveChanged: if (active)
                    root.startSystemMove()
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: root.nativeChrome && !root.sidebarVisible ? 84 : 16
                anchors.rightMargin: 16
                spacing: 8
                ViciButton {
                    id: showSidebarButton
                    visible: !root.sidebarVisible
                    implicitHeight: 28
                    iconSource: Img.icon(BuiltinIcon.AppWindowSidebarLeft).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("Show sidebar")
                    activeFocusOnTab: true
                    ToolTip.visible: hovered
                    ToolTip.text: accessibleName
                    ToolTip.delay: 600
                    onClicked: root.sidebarVisible = true
                }
                InlineEditableText {
                    id: titleEditor
                    property string editedConversationId: ""
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth
                    implicitHeight: 28
                    text: Chat.session?.title ?? qsTr("New chat")
                    accessibleName: qsTr("Conversation title")
                    readOnly: !Chat.session?.conversationId
                    horizontalAlignment: Text.AlignLeft
                    horizontalPadding: 8
                    font.pointSize: Theme.regularFontSize
                    font.weight: Font.Medium
                    onEditingChanged: if (editing)
                        editedConversationId = Chat.session.conversationId
                    onCommitted: value => Chat.renameConversation(editedConversationId, value)
                }
                Item {
                    Layout.fillWidth: true
                }
                ViciSpinner {
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    visible: Chat.loading
                }
                ViciButton {
                    id: newChatButton
                    visible: !root.sidebarVisible
                    implicitHeight: 28
                    iconSource: Img.icon(BuiltinIcon.Plus).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("New chat")
                    activeFocusOnTab: true
                    ToolTip.visible: hovered
                    ToolTip.text: accessibleName
                    ToolTip.delay: 600
                    onClicked: Chat.newChat()
                }
                ViciButton {
                    id: moreButton
                    implicitHeight: 28
                    visible: !!Chat.session?.conversationId
                    iconSource: Img.icon(BuiltinIcon.Ellipsis).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("Conversation actions")
                    activeFocusOnTab: true
                    onClicked: conversationMenu.popup(moreButton, 0, moreButton.height)
                    Menu {
                        id: conversationMenu
                        popupType: Popup.Native
                        MenuItem {
                            text: qsTr("Find in conversation…")
                            onTriggered: (conversation.item as ChatConversation)?.openFind()
                        }
                        MenuItem {
                            text: qsTr("Rename…")
                            onTriggered: titleEditor.startEditing()
                        }
                        MenuItem {
                            text: qsTr("Delete…")
                            onTriggered: root.confirmDelete(Chat.session.conversationId, Chat.session.title)
                        }
                    }
                }
            }
        }

        Loader {
            id: conversation
            anchors.top: toolbar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            enabled: !Chat.loading
            function loadSession() {
                active = false;
                setSource(Qt.resolvedUrl("ChatConversation.qml"), {
                    session: Chat.session,
                    horizontalPadding: Qt.binding(() => root.contentPadding),
                    topInset: Qt.binding(() => root.headerHeight)
                });
                active = true;
            }
            Component.onCompleted: loadSession()
        }

        Rectangle {
            anchors.top: toolbar.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width - 48, 560)
            height: errorRow.implicitHeight + 20
            visible: Chat.error.length > 0
            radius: 10
            color: Qt.tint(Theme.background, Config.withAlpha(Theme.danger, 0.10))
            border.color: Config.withAlpha(Theme.danger, 0.2)
            RowLayout {
                id: errorRow
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    Layout.fillWidth: true
                    text: Chat.error
                    wrapMode: Text.Wrap
                    color: Theme.foreground
                    font.family: Theme.fontFamily
                    font.pointSize: Theme.smallerFontSize
                }
                ViciButton {
                    text: qsTr("Dismiss")
                    onClicked: Chat.clearError()
                }
            }
        }
    }

    Rectangle {
        x: root.sidebarWidth
        width: 1
        height: parent.height
        visible: root.sidebarVisible
        color: Config.withAlpha(Theme.foreground, 0.04)
    }

    function confirmDelete(id, title) {
        deleteDialog.conversationId = id;
        deleteDialog.conversationTitle = title;
        deleteDialog.open();
    }

    ViciModal {
        id: deleteDialog
        property string conversationId: ""
        property string conversationTitle: ""
        parent: Overlay.overlay
        width: Math.min(parent.width - 40, 420)
        padding: 24
        onOpened: cancelDelete.forceActiveFocus()

        contentItem: ColumnLayout {
            spacing: 16
            Text {
                text: qsTr("Delete conversation?")
                color: Theme.foreground
                font.family: Theme.fontFamily
                font.pointSize: Theme.regularFontSize + 2
                font.weight: Font.DemiBold
            }
            Text {
                Layout.fillWidth: true
                text: qsTr("“%1” and its attachments will be permanently deleted.").arg(deleteDialog.conversationTitle)
                textFormat: Text.PlainText
                wrapMode: Text.Wrap
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pointSize: Theme.regularFontSize
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8
                ViciButton {
                    id: cancelDelete
                    text: qsTr("Cancel")
                    activeFocusOnTab: true
                    onClicked: deleteDialog.close()
                }
                ViciButton {
                    text: qsTr("Delete")
                    foreground: Theme.danger
                    variant: "tinted"
                    activeFocusOnTab: true
                    onClicked: {
                        Chat.deleteConversation(deleteDialog.conversationId);
                        deleteDialog.close();
                    }
                }
            }
        }
    }

    Connections {
        target: Chat
        function onSessionChanged() {
            titleEditor.finishEditing();
            conversation.loadSession();
        }
        function onFocusRequested() {
            const content = conversation.item as ChatConversation;
            if (content)
                content.focusComposer();
        }
    }
}
