import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 220

    property string _selectedKey: ""
    readonly property bool _searching: extSearchField.text.length > 0
    readonly property string _activeKey: _searching ? _selectedKey : settings.currentPage

    function _activate(key) {
        const model = settings.sidebarModel;
        const idx = model.indexOfKey(key);
        if (idx < 0)
            return;
        const wasSearching = _searching;
        if (model.kindAt(idx) === "command")
            settings.selectExtension(key);
        else
            settings.currentPage = key;
        extSearchField.text = "";
        if (wasSearching)
            Qt.callLater(() => {
                const i = settings.sidebarModel.indexOfKey(settings.currentPage);
                if (i >= 0)
                    navList.positionViewAtIndex(i, ListView.Beginning);
            });
    }

    function _move(delta) {
        const model = settings.sidebarModel;
        const next = model.stepRow(model.indexOfKey(_activeKey), delta);
        if (next < 0)
            return;
        const key = model.keyAt(next);
        if (root._searching)
            root._selectedKey = key;
        else
            settings.currentPage = key;
        navList.positionViewAtIndex(next, ListView.Contain);
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
                            text: qsTr("Search...")
                            color: Theme.textPlaceholder
                            font: extSearchField.font
                            visible: !extSearchField.text
                        }

                        onTextChanged: {
                            HoverActivation.reset();
                            settings.sidebarModel.setQuery(text);
                            root._selectedKey = text.length > 0 ? settings.sidebarModel.keyAt(settings.sidebarModel.firstSelectableRow()) : "";
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
            model: settings.sidebarModel

            ViciWheelHandler {
                target: navList
            }

            ScrollBar.vertical: ViciScrollBar {
                policy: navList.contentHeight > navList.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: navItem
                required property var model
                width: navList.width
                height: navItem.model.kind === "divider" ? 18 : 32

                readonly property string _key: navItem.model.key
                readonly property bool _isCommand: navItem.model.kind === "command"
                readonly property bool _selected: root._activeKey === navItem._key
                readonly property bool _enabled: navItem.model.enabled !== false

                ViciDivider {
                    visible: navItem.model.kind === "divider"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                }

                SourceBlendRect {
                    visible: navItem.model.kind !== "divider"
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
                        opacity: navItem._enabled ? 1.0 : 0.5

                        ViciImage {
                            source: navItem.model.kind === "core" ? Img.builtin(navItem.model.icon).withFillColor(navItem._selected ? Theme.listItemSelectionFg : Theme.textMuted) : navItem.model.iconSource
                            Layout.preferredWidth: navItem._isCommand ? 16 : 18
                            Layout.preferredHeight: navItem._isCommand ? 16 : 18
                        }

                        Text {
                            text: navItem.model.label
                            color: navItem._selected ? Theme.listItemSelectionFg : Theme.foreground
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
