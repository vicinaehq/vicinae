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
                font.family: Theme.fontFamily
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

                function _wordBoundaryBackward(text, pos) {
                    let i = pos - 1
                    while (i > 0 && /\s/.test(text[i])) i--
                    while (i > 0 && !/\s/.test(text[i - 1])) i--
                    return Math.max(0, i)
                }

                function _wordBoundaryForward(text, pos) {
                    let i = pos
                    while (i < text.length && !/\s/.test(text[i])) i++
                    while (i < text.length && /\s/.test(text[i])) i++
                    return i
                }

                function _syncSearchText() {
                    launcher.forwardSearchText(searchInput.text)
                    if (launcher.isRootSearch)
                        searchModel.setFilter(searchInput.text)
                }

                function _handleEmacsEditing(event) {
                    if (!Config.emacsMode) return false

                    const ctrl = (event.modifiers & Qt.ControlModifier)
                    const alt = (event.modifiers & Qt.AltModifier)
                    const noOther = !(event.modifiers & ~(Qt.ControlModifier | Qt.AltModifier | Qt.KeypadModifier | Qt.GroupSwitchModifier))

                    if (ctrl && !alt && noOther) {
                        switch (event.key) {
                        case Qt.Key_A:
                            searchInput.cursorPosition = 0
                            return true
                        case Qt.Key_E:
                            searchInput.cursorPosition = searchInput.text.length
                            return true
                        case Qt.Key_B:
                            if (searchInput.cursorPosition > 0)
                                searchInput.cursorPosition--
                            return true
                        case Qt.Key_F:
                            if (searchInput.cursorPosition < searchInput.text.length)
                                searchInput.cursorPosition++
                            return true
                        case Qt.Key_K: {
                            searchInput.text = searchInput.text.substring(0, searchInput.cursorPosition)
                            _syncSearchText()
                            return true
                        }
                        case Qt.Key_U: {
                            const pos = searchInput.cursorPosition
                            searchInput.text = searchInput.text.substring(pos)
                            searchInput.cursorPosition = 0
                            _syncSearchText()
                            return true
                        }
                        }
                    }

                    if (alt && !ctrl && noOther) {
                        switch (event.key) {
                        case Qt.Key_B: {
                            searchInput.cursorPosition = _wordBoundaryBackward(searchInput.text, searchInput.cursorPosition)
                            return true
                        }
                        case Qt.Key_F: {
                            searchInput.cursorPosition = _wordBoundaryForward(searchInput.text, searchInput.cursorPosition)
                            return true
                        }
                        case Qt.Key_Backspace: {
                            const pos = searchInput.cursorPosition
                            const boundary = _wordBoundaryBackward(searchInput.text, pos)
                            searchInput.text = searchInput.text.substring(0, boundary) + searchInput.text.substring(pos)
                            searchInput.cursorPosition = boundary
                            _syncSearchText()
                            return true
                        }
                        case Qt.Key_D: {
                            const pos = searchInput.cursorPosition
                            const boundary = _wordBoundaryForward(searchInput.text, pos)
                            searchInput.text = searchInput.text.substring(0, pos) + searchInput.text.substring(boundary)
                            searchInput.cursorPosition = pos
                            _syncSearchText()
                            return true
                        }
                        }
                    }

                    return false
                }

                function _handleNavigation(event) {
                    const nav = launcher.matchNavigationKey(event.key, event.modifiers)
                    if (nav === 0) return false

                    if (launcher.compacted) { launcher.expand(); return true }

                    if (nav === 1) {
                        if (commandStack.currentItem) commandStack.currentItem.moveUp()
                        else if (!launcher.isRootSearch) launcher.forwardKey(Qt.Key_Up)
                        else searchList.moveUp()
                    } else if (nav === 2) {
                        if (commandStack.currentItem) commandStack.currentItem.moveDown()
                        else if (!launcher.isRootSearch) launcher.forwardKey(Qt.Key_Down)
                        else searchList.moveDown()
                    } else if (nav === 3) {
                        if (commandStack.currentItem && typeof commandStack.currentItem.moveLeft === "function")
                            commandStack.currentItem.moveLeft()
                    } else if (nav === 4) {
                        if (commandStack.currentItem && typeof commandStack.currentItem.moveRight === "function")
                            commandStack.currentItem.moveRight()
                    }
                    return true
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
                    if (_handleEmacsEditing(event)) {
                        event.accepted = true
                    } else if (_handleNavigation(event)) {
                        event.accepted = true
                    } else if (event.key === Qt.Key_Backspace && searchInput.text === "" && !launcher.isRootSearch && launcher.showBackButton) {
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
