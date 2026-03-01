import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var model: settings.keybindModel

    property string _filterText: ""

    function _clearFilter() {
        _filterText = ""
        root.model.setFilter("")
    }

    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape) {
            if (_filterText) _clearFilter()
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Backspace) {
            _filterText = _filterText.slice(0, -1)
            root.model.setFilter(_filterText)
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Up) {
            root.model.moveUp()
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Down) {
            root.model.moveDown()
            event.accepted = true
            return
        }
        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            if (!root.model.hasSelection) return
            recorder.open()
            event.accepted = true
            return
        }

        const ctrl = event.modifiers & Qt.ControlModifier
        const noOther = !(event.modifiers & ~(Qt.ControlModifier | Qt.KeypadModifier | Qt.GroupSwitchModifier))
        if (ctrl && noOther) {
            if ((Config.emacsMode && event.key === Qt.Key_P)
                || (!Config.emacsMode && event.key === Qt.Key_K)) {
                root.model.moveUp()
                event.accepted = true
                return
            }
            if ((Config.emacsMode && event.key === Qt.Key_N)
                || (!Config.emacsMode && event.key === Qt.Key_J)) {
                root.model.moveDown()
                event.accepted = true
                return
            }
        }

        if (event.text && event.text.length === 1 && !event.modifiers) {
            _filterText += event.text
            root.model.setFilter(_filterText)
            event.accepted = true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Filter indicator
        Rectangle {
            visible: root._filterText !== ""
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.topMargin: 8
            Layout.bottomMargin: 4
            height: 28
            radius: 4
            color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.1)
            border.color: Theme.accent
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 6

                Image {
                    source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.accent
                    sourceSize.width: 12
                    sourceSize.height: 12
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                }

                Text {
                    text: root._filterText
                    color: Theme.foreground
                    font.pointSize: Theme.smallerFontSize
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: "Esc"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize - 1
                }
            }
        }

        ListView {
            id: keybindList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: keybindList.contentHeight > keybindList.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            Connections {
                target: root.model
                function onSelectedChanged() {
                    if (root.model.selectedRow >= 0)
                        keybindList.positionViewAtIndex(root.model.selectedRow, ListView.Contain)
                }
            }

            delegate: Item {
                id: rowItem
                width: keybindList.width
                height: Math.max(44, rowContent.implicitHeight + 16) + 1

                required property int index
                required property string name
                required property string icon
                required property string shortcut

                readonly property bool isSelected: index === root.model.selectedRow

                Rectangle {
                    anchors.fill: parent
                    anchors.bottomMargin: 1
                    color: rowItem.isSelected ? Theme.listItemSelectionBg
                           : rowHover.hovered ? Theme.listItemHoverBg
                           : "transparent"
                }

                HoverHandler { id: rowHover }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: {
                        root.model.select(rowItem.index)
                        root.forceActiveFocus()
                    }
                    onDoubleClicked: {
                        root.model.select(rowItem.index)
                        recorder.open()
                    }
                }

                RowLayout {
                    id: rowContent
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 12

                    ViciImage {
                        source: rowItem.icon
                            ? Img.builtin(rowItem.icon).withBackgroundTint("orange")
                            : ""
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        visible: rowItem.icon !== ""
                    }

                    Text {
                        text: rowItem.name
                        color: rowItem.isSelected ? Theme.listItemSelectionFg : Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Item {
                        id: shortcutBadgeContainer
                        Layout.alignment: Qt.AlignVCenter
                        implicitWidth: badgeRow.implicitWidth
                        implicitHeight: badgeRow.implicitHeight

                        Row {
                            id: badgeRow
                            anchors.verticalCenter: parent.verticalCenter

                            ShortcutBadge {
                                visible: rowItem.shortcut !== ""
                                text: rowItem.shortcut
                            }

                            Text {
                                visible: rowItem.shortcut === "" && rowHover.hovered
                                text: "Record Shortcut"
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                root.model.select(rowItem.index)
                                recorder.open()
                            }
                        }
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    height: 1
                    color: Theme.divider
                }
            }
        }
    }

    // Full-panel shortcut recorder overlay
    FocusScope {
        id: recorder
        anchors.fill: parent
        visible: false
        z: 10

        property string _currentShortcut: ""
        property string _statusText: "Press a shortcut..."
        property color _statusColor: Theme.textMuted

        function open() {
            _currentShortcut = ""
            _statusText = "Press a shortcut..."
            _statusColor = Theme.textMuted
            _closeTimer.stop()
            visible = true
            forceActiveFocus()
        }

        function close() {
            visible = false
            root.forceActiveFocus()
        }

        Timer {
            id: _closeTimer
            interval: 1500
            onTriggered: recorder.close()
        }

        Keys.onPressed: (event) => {
            event.accepted = true
            _closeTimer.stop()

            const key = event.key
            const mods = event.modifiers

            const isModKey = key === Qt.Key_Shift || key === Qt.Key_Control
                             || key === Qt.Key_Alt || key === Qt.Key_Meta
            const isCloseKey = key === Qt.Key_Escape || key === Qt.Key_Backspace

            if (!isModKey && isCloseKey && mods === Qt.NoModifier) {
                recorder.close()
                return
            }

            recorder._currentShortcut = root.model.shortcutDisplayString(key, mods)

            if (isModKey) {
                recorder._statusText = "Press a key..."
                recorder._statusColor = Theme.textMuted
                return
            }

            const error = root.model.validateShortcut(key, mods)
            if (error !== "") {
                recorder._statusText = error
                recorder._statusColor = Theme.danger
                return
            }

            root.model.setShortcut(root.model.selectedRow, key, mods)
            recorder._statusText = "Shortcut updated"
            recorder._statusColor = Theme.toastSuccess
            _closeTimer.start()
        }

        // Dimmed backdrop
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.85)

            MouseArea {
                anchors.fill: parent
                onClicked: recorder.close()
            }
        }

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 16
            width: Math.min(parent.width - 80, 400)

            Text {
                visible: root.model.hasSelection
                text: root.model.selectedName
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            ShortcutBadge {
                visible: recorder._currentShortcut !== ""
                text: recorder._currentShortcut
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: recorder._statusText
                color: recorder._statusColor
                font.pointSize: Theme.regularFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            Text {
                text: "Press Esc to cancel"
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }
}
