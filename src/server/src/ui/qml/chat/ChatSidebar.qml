pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    property bool nativeChrome: false
    required property real headerHeight
    signal hideRequested
    signal moveRequested
    readonly property Item titlebarControls: toolbarButtons
    property alias searchButton: searchButton
    signal deleteRequested(string conversationId, string title)

    readonly property string activeConversationId: Chat.session?.conversationId ?? ""

    function revealConversation() {
        if (!visible)
            return;
        history.forceLayout();
        history.currentIndex = Chat.conversations.indexOf(activeConversationId);
        if (history.currentIndex >= 0) {
            history.cancelFlick();
            history.positionViewAtIndex(history.currentIndex, ListView.Contain);
        }
    }

    onActiveConversationIdChanged: Qt.callLater(root.revealConversation)
    onVisibleChanged: if (visible)
        Qt.callLater(root.revealConversation)
    Component.onCompleted: Qt.callLater(root.revealConversation)

    Connections {
        target: Chat
        function onFocusRequested() {
            Qt.callLater(root.revealConversation);
        }
    }

    Connections {
        target: Chat.conversations
        function onModelReset() {
            Qt.callLater(root.revealConversation);
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        anchors.bottomMargin: 12
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: root.headerHeight
            spacing: 4

            DragHandler {
                enabled: root.nativeChrome
                target: null
                grabPermissions: PointerHandler.CanTakeOverFromHandlersOfSameType | PointerHandler.CanTakeOverFromHandlersOfDifferentType
                onActiveChanged: if (active)
                    root.moveRequested()
            }

            Item {
                Layout.fillWidth: true
            }
            RowLayout {
                id: toolbarButtons
                spacing: 4
                ViciButton {
                    implicitHeight: 28
                    iconSource: Img.icon(BuiltinIcon.AppWindowSidebarLeft).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("Hide sidebar")
                    activeFocusOnTab: true
                    ToolTip.visible: hovered
                    ToolTip.text: accessibleName
                    ToolTip.delay: 600
                    onClicked: root.hideRequested()
                }
                ViciButton {
                    id: searchButton
                    implicitHeight: 28
                    iconSource: Img.icon(BuiltinIcon.MagnifyingGlass).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("Search conversations")
                    activeFocusOnTab: true
                    ToolTip.visible: hovered
                    ToolTip.text: accessibleName
                    ToolTip.delay: 600
                    onClicked: Chat.searchConversations(searchButton)
                }
                ViciButton {
                    implicitHeight: 28
                    iconSource: Img.icon(BuiltinIcon.Plus).withFillColor(Theme.textMuted)
                    accessibleName: qsTr("New chat")
                    activeFocusOnTab: true
                    ToolTip.visible: hovered
                    ToolTip.text: accessibleName
                    ToolTip.delay: 600
                    onClicked: Chat.newChat()
                }
            }
        }

        ListView {
            id: history
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: Chat.conversations
            currentIndex: -1
            reuseItems: true
            cacheBuffer: 128
            spacing: 2
            boundsBehavior: Flickable.StopAtBounds
            activeFocusOnTab: true
            keyNavigationEnabled: true
            highlightMoveDuration: 0
            layer.enabled: contentHeight > height
            layer.effect: ScrollFade {
                topFade: Math.min(12, Math.max(0, history.contentY - history.originY))
                bottomFade: Math.min(12, Math.max(0, history.originY + history.contentHeight - history.contentY - history.height))
            }
            section.property: "dateGroup"
            section.delegate: Text {
                required property string section
                width: history.width
                height: 28
                leftPadding: 10
                topPadding: 8
                text: section
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
                font.weight: Font.Medium
            }

            ScrollBar.vertical: ViciScrollBar {}

            delegate: Rectangle {
                id: row
                required property int index
                required property string conversationId
                required property string title
                readonly property bool selected: Chat.session?.conversationId === conversationId
                property bool pooled: false
                ListView.onPooled: {
                    pooled = true;
                    titleEditor.cancelEditing();
                }
                ListView.onReused: pooled = false
                width: history.width
                height: 32
                radius: 6
                color: selected ? Config.withAlpha(Theme.foreground, 0.10) : rowHover.hovered ? Config.withAlpha(Theme.foreground, 0.04) : "transparent"
                border.width: ListView.isCurrentItem && history.activeFocus ? 1 : 0
                border.color: Theme.inputBorderFocus
                Accessible.role: Accessible.ListItem
                Accessible.name: title
                Accessible.selected: selected
                Accessible.ignored: row.pooled || titleEditor.editing
                Accessible.onPressAction: Chat.openConversation(conversationId)

                HoverHandler {
                    id: rowHover
                    enabled: !row.pooled && !titleEditor.editing
                }
                TapHandler {
                    enabled: !row.pooled && !titleEditor.editing
                    onTapped: {
                        history.currentIndex = row.index;
                        Chat.openConversation(row.conversationId);
                    }
                }
                InlineEditableText {
                    id: titleEditor
                    anchors.fill: parent
                    text: row.title
                    accessibleName: qsTr("Conversation title")
                    horizontalAlignment: Text.AlignLeft
                    horizontalPadding: 10
                    clickToEdit: false
                    foreground: row.selected ? Theme.foreground : Config.withAlpha(Theme.foreground, 0.78)
                    font.pointSize: Theme.regularFontSize
                    font.weight: row.selected ? Font.Medium : Font.Normal
                    onCommitted: value => Chat.renameConversation(row.conversationId, value)
                }
                ContextMenu.menu: Menu {
                    popupType: Popup.Native
                    MenuItem {
                        text: qsTr("Rename…")
                        onTriggered: titleEditor.startEditing()
                    }
                    MenuItem {
                        text: qsTr("Delete…")
                        onTriggered: root.deleteRequested(row.conversationId, row.title)
                    }
                }
            }
            Keys.onReturnPressed: {
                if (currentItem)
                    Chat.openConversation(Chat.conversations.idAt(currentIndex));
            }

            Text {
                anchors.top: parent.top
                anchors.topMargin: 28
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 24
                visible: history.count === 0
                text: qsTr("Your conversations will appear here.")
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize
            }
        }

        ViciButton {
            Layout.alignment: Qt.AlignLeft
            implicitHeight: 28
            iconSource: Img.icon(BuiltinIcon.Cog).withFillColor(Theme.textMuted)
            accessibleName: qsTr("AI settings")
            activeFocusOnTab: true
            ToolTip.visible: hovered
            ToolTip.text: accessibleName
            ToolTip.delay: 600
            onClicked: Chat.openSettings()
        }
    }
}
