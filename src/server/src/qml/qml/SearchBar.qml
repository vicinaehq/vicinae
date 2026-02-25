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
        spacing: launcher.hasCompleter ? 4 : 12

        Rectangle {
            id: backButton
            visible: !launcher.isRootSearch && launcher.showBackButton
            Layout.preferredWidth: 28
            Layout.preferredHeight: 28
            Layout.alignment: Qt.AlignVCenter
            radius: 6
            color: backMouseArea.containsMouse ? Qt.lighter(Theme.buttonPrimaryBg, 1.3) : Theme.buttonPrimaryBg
            Behavior on color { ColorAnimation { duration: 100 } }

            ViciImage {
                anchors.centerIn: parent
                width: 14
                height: 14
                source: Img.builtin("arrow-left")
            }

            MouseArea {
                id: backMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: launcher.goBack()
            }
        }

        Item {
            id: searchInputContainer
            Layout.fillWidth: !launcher.hasCompleter
            Layout.preferredWidth: launcher.hasCompleter ? searchInputMetrics.advanceWidth : -1
            Layout.fillHeight: true

            TextMetrics {
                id: searchInputMetrics
                font: searchInput.font
                text: searchInput.text || " "
            }

            TextInput {
                id: searchInput
                anchors.fill: parent
                verticalAlignment: TextInput.AlignVCenter
                font.pointSize: Theme.regularFontSize * 1.1
                color: Theme.foreground
                selectionColor: Theme.textSelectionBg
                selectedTextColor: Theme.textSelectionFg
                clip: true
                readOnly: !launcher.searchInteractive

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    text: launcher.hasCompleter ? "..."
                        : !launcher.isRootSearch && launcher.searchPlaceholder !== ""
                          ? launcher.searchPlaceholder : "Search for anything..."
                    color: Theme.textPlaceholder
                    font: searchInput.font
                    visible: !searchInput.text && launcher.searchInteractive
                }

                onTextEdited: {
                    launcher.forwardSearchText(text)
                    if (launcher.isRootSearch) {
                        searchModel.setFilter(text)
                    }
                }

                Keys.onUpPressed: {
                    if (launcher.compacted) { launcher.expand(); return }
                    if (commandStack.currentItem) commandStack.currentItem.moveUp()
                    else if (!launcher.isRootSearch) launcher.forwardKey(Qt.Key_Up)
                    else searchList.moveUp()
                }
                Keys.onDownPressed: {
                    if (launcher.compacted) { launcher.expand(); return }
                    if (commandStack.currentItem) commandStack.currentItem.moveDown()
                    else if (!launcher.isRootSearch) launcher.forwardKey(Qt.Key_Down)
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
                    if (launcher.compacted) { launcher.expand(); event.accepted = true; return }
                    if (event.modifiers !== Qt.NoModifier) {
                        event.accepted = launcher.forwardKey(event.key, event.modifiers)
                    } else {
                        launcher.handleReturn()
                    }
                }
                Keys.onBacktabPressed: (event) => { event.accepted = false }
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Backspace && searchInput.text === "" && !launcher.isRootSearch && launcher.showBackButton) {
                        launcher.goBack()
                        event.accepted = true
                    } else if (event.key === Qt.Key_Space && launcher.isRootSearch && event.modifiers === Qt.NoModifier) {
                        if (launcher.tryAliasFastTrack()) {
                            event.accepted = true
                        }
                    } else if (launcher.forwardKey(event.key, event.modifiers)) {
                        if (launcher.compacted) launcher.expand()
                        event.accepted = true
                    }
                }
            }
        }

        ArgCompleter {
            id: argCompleter
            visible: launcher.hasCompleter
            args: launcher.completerArgs
            icon: launcher.completerIcon
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter

            onValueChanged: (index, value) => {
                launcher.setCompleterValue(index, value)
            }
            onFocusSearchInput: searchInput.forceActiveFocus()
        }

        Loader {
            id: accessoryLoader
            active: launcher.searchAccessoryUrl.toString() !== ""
            source: launcher.searchAccessoryUrl
            visible: active
            Layout.alignment: Qt.AlignVCenter
        }

        Shortcut {
            sequence: Keybinds.openSearchAccessory
            enabled: !!accessoryLoader.item
            onActivated: {
                if (typeof accessoryLoader.item.open === "function")
                    accessoryLoader.item.open()
            }
        }

        Connections {
            target: accessoryLoader.item
            ignoreUnknownSignals: true
            function onPopupClosed() { searchInput.forceActiveFocus() }
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
                if (launcher.isRootSearch) {
                    searchModel.setFilter(text)
                }
            }
        }
        function onViewNavigatedBack() {
            root.focusInput()
        }
        function onCompleterChanged() {
            if (!launcher.hasCompleter && !searchInput.activeFocus) {
                searchInput.forceActiveFocus()
            }
        }
        function onCompleterValidationFailed() {
            argCompleter.validate()
        }
        function onCompleterValuesChanged() {
            if (launcher.hasCompleter)
                argCompleter.setValues(launcher.completerValues)
        }
    }
}
