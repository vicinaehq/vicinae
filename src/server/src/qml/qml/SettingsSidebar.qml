import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 220

    property string _selectedKey: ""
    readonly property bool _searching: extSearchField.text.length > 0
    readonly property string _activeKey: _searching ? _selectedKey : settings.currentPage

    readonly property var _allItems: {
        settings.sidebarExtensions; // re-evaluate when extensions change
        return settings.filterSidebarItems(extSearchField.text);
    }

    readonly property var _navItems: {
        let nav = [];
        for (let i = 0; i < _allItems.length; i++) {
            const it = _allItems[i];
            if (it._kind !== "divider")
                nav.push({
                    listIndex: i,
                    key: it.id,
                    kind: it._kind
                });
        }
        return nav;
    }

    function _navIndexOfKey(key) {
        for (let i = 0; i < _navItems.length; i++) {
            if (_navItems[i].key === key)
                return i;
        }
        return -1;
    }

    function _activate(key) {
        const idx = _navIndexOfKey(key);
        if (idx < 0)
            return;
        const wasSearching = _searching;
        if (_navItems[idx].kind === "command")
            settings.selectExtension(key);
        else
            settings.currentPage = key;
        extSearchField.text = "";
        if (wasSearching)
            Qt.callLater(() => {
                const i = root._navIndexOfKey(settings.currentPage);
                if (i >= 0)
                    navList.positionViewAtIndex(root._navItems[i].listIndex, ListView.Beginning);
            });
    }

    function _move(delta) {
        if (_navItems.length === 0)
            return;
        const cur = _navIndexOfKey(_activeKey);
        const next = cur < 0 ? (delta > 0 ? 0 : _navItems.length - 1) : Math.max(0, Math.min(_navItems.length - 1, cur + delta));
        const item = _navItems[next];
        if (root._searching)
            root._selectedKey = item.key;
        else
            settings.currentPage = item.key;
        navList.positionViewAtIndex(item.listIndex, ListView.Contain);
    }

    // Filter directly; the _navItems binding can be stale inside onTextChanged.
    function _firstMatchKey(query) {
        const items = settings.filterSidebarItems(query);
        for (let i = 0; i < items.length; i++) {
            if (items[i]._kind !== "divider")
                return items[i].id;
        }
        return "";
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 46

            SourceBlendRect {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                height: 30
                radius: 8
                backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
                color: Config.withAlpha(Theme.secondaryBackground, Config.windowOpacity)

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 6

                    ViciImage {
                        source: Img.builtin("magnifying-glass").withFillColor(Theme.textMuted)
                        sourceSize.width: 14
                        sourceSize.height: 14
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                    }

                    TextInput {
                        id: extSearchField
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        verticalAlignment: TextInput.AlignVCenter
                        font.pointSize: Theme.regularFontSize
                        color: Theme.foreground
                        clip: true
                        focus: true
                        activeFocusOnTab: true

                        Connections {
                            target: settings
                            function onDefaultFocusRequested() {
                                extSearchField.forceActiveFocus();
                            }
                        }

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Search..."
                            color: Theme.textPlaceholder
                            font: extSearchField.font
                            visible: !extSearchField.text
                        }

                        onTextChanged: {
                            HoverActivation.reset();
                            root._selectedKey = text.length > 0 ? root._firstMatchKey(text) : "";
                        }

                        Keys.onUpPressed: root._move(-1)
                        Keys.onDownPressed: root._move(1)
                        Keys.onReturnPressed: root._activate(root._activeKey)
                        Keys.onEnterPressed: root._activate(root._activeKey)
                        Keys.onPressed: event => {
                            if (event.key === Qt.Key_Escape && text) {
                                text = "";
                                event.accepted = true;
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
            bottomMargin: 8
            topMargin: 2
            boundsBehavior: Flickable.StopAtBounds
            model: root._allItems

            ScrollBar.vertical: ViciScrollBar {
                policy: navList.contentHeight > navList.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: navItem
                required property var modelData
                required property int index
                width: navList.width
                height: modelData._kind === "divider" ? 18 : 32

                readonly property string _key: modelData.id ?? ""
                readonly property bool _isCommand: modelData._kind === "command"
                readonly property bool _selected: root._activeKey === navItem._key
                property bool _isEnabled: modelData.enabled !== false

                Connections {
                    target: settings
                    function onSidebarItemEnabledChanged(providerId, enabled) {
                        if (providerId === navItem._key)
                            navItem._isEnabled = enabled;
                    }
                }

                ViciDivider {
                    visible: navItem.modelData._kind === "divider"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                }

                SourceBlendRect {
                    visible: navItem.modelData._kind !== "divider"
                    anchors.fill: parent
                    anchors.leftMargin: 6
                    anchors.rightMargin: 6
                    anchors.topMargin: 1
                    anchors.bottomMargin: 1
                    radius: 8
                    backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
                    color: {
                        if (navItem._selected) {
                            const c = Theme.listItemSelectionBg;
                            return Qt.rgba(c.r, c.g, c.b, Config.windowOpacity);
                        }
                        if (itemHover.hovered && HoverActivation.active) {
                            const h = Theme.listItemHoverBg;
                            return Qt.rgba(h.r, h.g, h.b, Config.windowOpacity);
                        }
                        return Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity);
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8 + (navItem._isCommand ? 12 : 0)
                        anchors.rightMargin: 8
                        spacing: 10

                        ViciImage {
                            source: navItem.modelData._kind === "core" ? Img.builtin(navItem.modelData.icon).withFillColor(navItem._selected ? Theme.listItemSelectionFg : Theme.textMuted) : (navItem.modelData.iconSource ?? "")
                            Layout.preferredWidth: navItem._isCommand ? 16 : 18
                            Layout.preferredHeight: navItem._isCommand ? 16 : 18
                            opacity: navItem._isEnabled ? 1.0 : 0.4
                        }

                        Text {
                            text: navItem.modelData.label ?? ""
                            color: navItem._selected ? Theme.listItemSelectionFg : navItem._isEnabled ? Theme.foreground : Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }

                    HoverHandler {
                        id: itemHover
                    }
                    TapHandler {
                        onTapped: root._activate(navItem._key)
                    }
                }
            }
        }
    }
}
