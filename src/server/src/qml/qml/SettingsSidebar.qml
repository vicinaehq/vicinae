import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 220

    property int _highlightedIndex: -1

    readonly property var _allItems: {
        settings.sidebarExtensions; // re-evaluate when extensions change
        return settings.filterSidebarItems(extSearchField.text);
    }

    readonly property var _navItems: {
        let nav = []
        for (let i = 0; i < _allItems.length; i++) {
            if (_allItems[i]._kind !== "divider")
                nav.push({ listIndex: i, pageId: _allItems[i].id })
        }
        return nav
    }

    function _navigateHighlight(delta) {
        if (_navItems.length === 0) return
        if (_highlightedIndex < 0) {
            _highlightedIndex = delta > 0 ? 0 : _navItems.length - 1
        } else {
            _highlightedIndex = Math.max(0, Math.min(_navItems.length - 1, _highlightedIndex + delta))
        }
    }

    function _activateHighlighted() {
        if (_highlightedIndex >= 0 && _highlightedIndex < _navItems.length)
            settings.currentPage = _navItems[_highlightedIndex].pageId
    }

    function _highlightedListIndex() {
        if (_highlightedIndex < 0 || _highlightedIndex >= _navItems.length) return -1
        return _navItems[_highlightedIndex].listIndex
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.topMargin: 12
            Layout.bottomMargin: 8
            height: 28
            radius: 4
            color: "transparent"
            border.color: extSearchField.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 4

                Image {
                    source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                    sourceSize.width: 12
                    sourceSize.height: 12
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                }

                TextInput {
                    id: extSearchField
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: TextInput.AlignVCenter
                    font.pointSize: Theme.smallerFontSize
                    color: Theme.foreground
                    clip: true
                    focus: true
                    activeFocusOnTab: true

                    Connections {
                        target: settings
                        function onDefaultFocusRequested() { extSearchField.forceActiveFocus() }
                    }

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        text: "Search..."
                        color: Theme.textPlaceholder
                        font: extSearchField.font
                        visible: !extSearchField.text
                    }

                    onTextChanged: root._highlightedIndex = root._navItems.length > 0 ? 0 : -1

                    Keys.onUpPressed: root._navigateHighlight(-1)
                    Keys.onDownPressed: root._navigateHighlight(1)
                    Keys.onReturnPressed: root._activateHighlighted()
                    Keys.onEnterPressed: root._activateHighlighted()
                    Keys.onPressed: (event) => {
                        if (event.key === Qt.Key_Escape) {
                            if (text) {
                                text = ""
                                event.accepted = true
                            } else if (root._highlightedIndex >= 0) {
                                root._highlightedIndex = -1
                                event.accepted = true
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id: navList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: root._allItems

            ScrollBar.vertical: ViciScrollBar {
                policy: navList.contentHeight > navList.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: navItem
                required property var modelData
                required property int index
                width: navList.width
                height: modelData._kind === "divider" ? 9 : 32

                readonly property bool _isHighlighted: index === root._highlightedListIndex()
                readonly property string _pageId: modelData.id ?? ""
                readonly property bool _isEnabled: modelData.enabled !== false

                Rectangle {
                    visible: navItem.modelData._kind === "divider"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    height: 1
                    color: Theme.divider
                }

                SourceBlendRect {
                    visible: navItem.modelData._kind !== "divider"
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    radius: 6
                    backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
                    color: {
                        if (settings.currentPage === navItem._pageId) {
                            const c = Theme.listItemSelectionBg
                            return Qt.rgba(c.r, c.g, c.b, Config.windowOpacity)
                        }
                        if (navItem._isHighlighted || itemHover.hovered) {
                            const h = Theme.listItemHoverBg
                            return Qt.rgba(h.r, h.g, h.b, Config.windowOpacity)
                        }
                        return Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 8

                        ViciImage {
                            source: navItem.modelData._kind === "core"
                                ? Img.builtin(navItem.modelData.icon).withFillColor(
                                    settings.currentPage === navItem._pageId ? Theme.listItemSelectionFg : Theme.textMuted)
                                : (navItem.modelData.iconSource ?? "")
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                            opacity: navItem._isEnabled ? 1.0 : 0.4
                        }

                        Text {
                            text: navItem.modelData.label ?? ""
                            color: settings.currentPage === navItem._pageId
                                   ? Theme.listItemSelectionFg
                                   : navItem._isEnabled ? Theme.foreground : Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        ViciImage {
                            visible: {
                                const p = navItem.modelData.provenance ?? ""
                                return p === "Raycast" || p === "Vicinae"
                            }
                            source: {
                                const p = navItem.modelData.provenance ?? ""
                                if (p === "Raycast") return Img.builtin("raycast").withFillColor(Theme.toastDanger)
                                if (p === "Vicinae") return Img.builtin("vicinae").withFillColor(Theme.toastWarning)
                                return ""
                            }
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                        }
                    }

                    HoverHandler { id: itemHover }
                    TapHandler {
                        onTapped: settings.currentPage = navItem._pageId
                    }
                }
            }
        }
    }
}
