import QtQuick
import QtQuick.Layouts

Item {
    id: root

    function focusInput() {
        if (!launcher.searchInteractive) return
        searchInput.forceActiveFocus()
        searchInput.selectAll()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 12

        // Back button — visible when not on root search
        Item {
            id: backButton
            visible: launcher.hasCommandView
            Layout.preferredWidth: 24
            Layout.preferredHeight: 24
            Layout.alignment: Qt.AlignVCenter
            opacity: backMouseArea.containsMouse ? 1.0 : 0.5

            ViciImage {
                anchors.centerIn: parent
                width: 16
                height: 16
                source: Img.builtin("chevron-left")
            }

            MouseArea {
                id: backMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: launcher.goBack()
            }
        }

        TextInput {
            id: searchInput
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: TextInput.AlignVCenter
            font.pointSize: Theme.regularFontSize
            color: Theme.foreground
            selectionColor: Theme.accent
            clip: true
            readOnly: !launcher.searchInteractive

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: launcher.hasCommandView && launcher.searchPlaceholder !== ""
                      ? launcher.searchPlaceholder : "Search..."
                color: Theme.textPlaceholder
                font: searchInput.font
                visible: !searchInput.text && launcher.searchInteractive
            }

            onTextEdited: {
                launcher.forwardSearchText(text)
                if (!launcher.hasCommandView) {
                    searchModel.setFilter(text)
                }
            }

            Keys.onUpPressed: {
                if (commandStack.currentItem) commandStack.currentItem.moveUp()
                else if (launcher.hasCommandView) launcher.forwardKey(Qt.Key_Up)
                else searchList.moveUp()
            }
            Keys.onDownPressed: {
                if (commandStack.currentItem) commandStack.currentItem.moveDown()
                else if (launcher.hasCommandView) launcher.forwardKey(Qt.Key_Down)
                else searchList.moveDown()
            }
            Keys.onLeftPressed: (event) => {
                if (commandStack.currentItem && typeof commandStack.currentItem.moveLeft === "function") {
                    commandStack.currentItem.moveLeft()
                    event.accepted = true
                } else {
                    event.accepted = false
                }
            }
            Keys.onRightPressed: (event) => {
                if (commandStack.currentItem && typeof commandStack.currentItem.moveRight === "function") {
                    commandStack.currentItem.moveRight()
                    event.accepted = true
                } else {
                    event.accepted = false
                }
            }
            Keys.onReturnPressed: (event) => {
                // Forward Shift+Enter and other modified returns for action shortcuts
                if (event.modifiers !== Qt.NoModifier) {
                    launcher.forwardKey(event.key, event.modifiers)
                    event.accepted = true
                } else {
                    launcher.handleReturn()
                }
            }
            Keys.onBacktabPressed: (event) => { event.accepted = false }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Backspace && searchInput.text === "" && launcher.hasCommandView) {
                    launcher.goBack()
                    event.accepted = true
                } else if (event.key === Qt.Key_Space && !launcher.hasCommandView && event.modifiers === Qt.NoModifier) {
                    if (launcher.tryAliasFastTrack()) {
                        event.accepted = true
                    }
                } else if (event.modifiers !== Qt.NoModifier && event.modifiers !== Qt.ShiftModifier
                           && event.key !== Qt.Key_Shift && event.key !== Qt.Key_Control
                           && event.key !== Qt.Key_Alt && event.key !== Qt.Key_Meta) {
                    // Forward modifier key combos for action shortcut matching
                    launcher.forwardKey(event.key, event.modifiers)
                    event.accepted = true
                }
            }
        }

        Loader {
            id: accessoryLoader
            active: launcher.searchAccessoryUrl.toString() !== ""
            source: launcher.searchAccessoryUrl
            visible: active
            Layout.alignment: Qt.AlignVCenter
        }
    }

    Connections {
        target: launcher
        function onSearchVisibleChanged() {
            if (launcher.searchVisible && launcher.searchInteractive)
                searchInput.forceActiveFocus()
        }
        function onSearchInteractiveChanged() {
            if (launcher.searchInteractive && launcher.searchVisible)
                searchInput.forceActiveFocus()
        }
        function onSearchTextUpdated(text) {
            if (searchInput.text !== text) {
                searchInput.text = text
                if (!launcher.hasCommandView) {
                    searchModel.setFilter(text)
                }
            }
        }
        function onViewNavigatedBack() {
            root.focusInput()
        }
        function onOpenSearchAccessoryRequested() {
            if (accessoryLoader.item && typeof accessoryLoader.item.open === "function")
                accessoryLoader.item.open()
        }
    }
}
