import QtQuick
import QtQuick.Layouts

Item {
    id: root

    function focusInput() {
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

            Image {
                anchors.centerIn: parent
                width: 16
                height: 16
                source: "image://vicinae/builtin:chevron-left"
                sourceSize.width: 16
                sourceSize.height: 16
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

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: launcher.hasCommandView && launcher.searchPlaceholder !== ""
                      ? launcher.searchPlaceholder : "Search..."
                color: Theme.textPlaceholder
                font: searchInput.font
                visible: !searchInput.text
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
            Keys.onReturnPressed: launcher.handleReturn()
            Keys.onBacktabPressed: (event) => { event.accepted = false }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Backspace && searchInput.text === "" && launcher.hasCommandView) {
                    launcher.goBack()
                    event.accepted = true
                } else if (event.key === Qt.Key_Space && !launcher.hasCommandView && event.modifiers === Qt.NoModifier) {
                    if (launcher.tryAliasFastTrack()) {
                        event.accepted = true
                    }
                }
            }
        }
    }

    Connections {
        target: launcher
        function onSearchTextUpdated(text) {
            if (searchInput.text !== text) {
                searchInput.text = text
                if (!launcher.hasCommandView) {
                    searchModel.setFilter(text)
                }
            }
        }
        function onViewNavigatedBack() {
            searchInput.selectAll()
        }
    }
}
