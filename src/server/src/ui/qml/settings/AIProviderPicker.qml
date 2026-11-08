pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciModal {
    id: root

    required property AISettingsModel model

    signal chosen(string type, string label, var icon, bool allowMultiple)

    property var _items: []
    property string query: ""
    property int selectedIndex: 0

    readonly property var filtered: {
        const q = root.query.trim().toLowerCase();
        const sections = [];
        for (const source of root._items) {
            if (q.length > 0 && !source.label.toLowerCase().includes(q) && !source.description.toLowerCase().includes(q) && !source.category.toLowerCase().includes(q))
                continue;
            let section = sections.find(s => s.category === source.category);
            if (!section) {
                section = {
                    category: source.category,
                    items: []
                };
                sections.push(section);
            }
            section.items.push(source);
        }
        // Keyboard navigation walks the items as they are displayed, section by section
        const items = [];
        for (const section of sections) {
            section.items = section.items.map(source => {
                const item = Object.assign({
                    index: items.length
                }, source);
                items.push(item);
                return item;
            });
        }
        return {
            sections: sections,
            items: items
        };
    }
    readonly property var sections: filtered.sections
    readonly property var items: filtered.items

    onQueryChanged: {
        HoverActivation.reset();
        selectedIndex = 0;
    }

    function openPicker() {
        _items = root.model.pickerTypes();
        searchInput.text = "";
        query = "";
        selectedIndex = 0;
        open();
    }

    function pick(item) {
        if (item.missing)
            return;
        close();
        root.chosen(item.type, item.label, item.icon, item.allowMultiple);
    }

    function pickSelected() {
        if (root.selectedIndex >= 0 && root.selectedIndex < root.items.length)
            pick(root.items[root.selectedIndex]);
    }

    function moveSelection(delta) {
        if (root.items.length === 0)
            return;
        HoverActivation.reset();
        selectedIndex = Math.max(0, Math.min(root.items.length - 1, selectedIndex + delta));
    }

    parent: Overlay.overlay
    width: Math.min(parent.width - 40, 580)
    height: Math.min(parent.height - 80, 520)
    padding: 0
    onOpened: searchInput.forceActiveFocus()

    contentItem: ColumnLayout {
        spacing: 0

        Item {
            Layout.fillWidth: true
            implicitHeight: 52

            TextInput {
                id: searchInput
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                verticalAlignment: TextInput.AlignVCenter
                font.family: Theme.fontFamily
                font.pointSize: Theme.regularFontSize * 1.2
                color: Theme.foreground
                selectionColor: Theme.textSelectionBg
                selectedTextColor: Theme.textSelectionFg
                clip: true
                onTextChanged: root.query = text
                Keys.onUpPressed: root.moveSelection(-1)
                Keys.onDownPressed: root.moveSelection(1)
                Keys.onReturnPressed: root.pickSelected()
                Keys.onEnterPressed: root.pickSelected()

                HoverHandler {
                    cursorShape: Qt.IBeamCursor
                }

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    text: qsTr("Search providers…")
                    color: Theme.textPlaceholder
                    font: searchInput.font
                    visible: !searchInput.displayText
                }
            }
        }

        ViciDivider {
            Layout.fillWidth: true
        }

        Flickable {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: column.implicitHeight
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            function ensureVisible(item) {
                const y = item.mapToItem(column, 0, 0).y;
                if (y < contentY)
                    contentY = y;
                else if (y + item.height > contentY + height)
                    contentY = y + item.height - height;
            }

            ViciWheelHandler {
                target: list
            }

            ScrollBar.vertical: ViciScrollBar {
                policy: list.contentHeight > list.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            ColumnLayout {
                id: column
                width: list.width
                spacing: 0

                Text {
                    visible: root.sections.length === 0
                    text: qsTr("No providers match.")
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    Layout.margins: 16
                }

                Repeater {
                    model: root.sections

                    delegate: ColumnLayout {
                        id: section
                        required property var modelData
                        Layout.fillWidth: true
                        spacing: 0

                        SettingsSectionLabel {
                            text: section.modelData.category
                            Layout.leftMargin: 20
                            Layout.topMargin: 12
                            Layout.bottomMargin: 4
                        }

                        Repeater {
                            model: section.modelData.items

                            delegate: Rectangle {
                                id: row
                                required property var modelData
                                readonly property bool selected: root.selectedIndex === modelData.index
                                readonly property bool hovered: rowHover.hovered && HoverActivation.active
                                readonly property bool missing: modelData.missing.length > 0
                                Layout.fillWidth: true
                                Layout.leftMargin: 8
                                Layout.rightMargin: 8
                                implicitHeight: 50
                                radius: 8
                                color: selected ? Theme.listItemSelectionBg : hovered ? Theme.listItemHoverBg : "transparent"

                                onSelectedChanged: if (selected)
                                    list.ensureVisible(row)

                                HoverHandler {
                                    id: rowHover
                                    cursorShape: row.missing ? Qt.ArrowCursor : Qt.PointingHandCursor
                                }

                                TapHandler {
                                    onTapped: root.pick(row.modelData)
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    opacity: row.missing ? 0.5 : 1
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 12
                                    spacing: 12

                                    ViciImage {
                                        source: row.modelData.icon.withFillColor(row.selected ? Theme.listItemSelectionFg : Theme.foreground)
                                        Layout.preferredWidth: 22
                                        Layout.preferredHeight: 22
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2

                                        Text {
                                            text: row.modelData.label
                                            color: row.selected ? Theme.listItemSelectionFg : Theme.foreground
                                            font.pointSize: Theme.regularFontSize
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            id: description
                                            text: row.modelData.description
                                            color: row.selected ? Theme.listItemSelectionFg : Theme.textMuted
                                            opacity: row.selected ? 0.8 : 1
                                            font.pointSize: Theme.smallerFontSize
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true

                                            HoverHandler {
                                                id: descriptionHover
                                            }

                                            ViciToolTip {
                                                text: description.text
                                                visible: descriptionHover.hovered && description.truncated
                                            }
                                        }
                                    }

                                    Text {
                                        visible: row.missing
                                        text: row.modelData.missing
                                        color: row.selected ? Theme.listItemSelectionFg : Theme.textPlaceholder
                                        font.pointSize: Theme.smallerFontSize
                                    }

                                    Text {
                                        visible: row.modelData.configured && !row.missing
                                        text: qsTr("Add another")
                                        color: row.selected ? Theme.listItemSelectionFg : Theme.textPlaceholder
                                        font.pointSize: Theme.smallerFontSize
                                    }

                                    ViciImage {
                                        visible: !row.missing
                                        source: Img.icon(BuiltinIcon.ChevronRightSmall).withFillColor(row.selected ? Theme.listItemSelectionFg : Theme.textMuted)
                                        Layout.preferredWidth: 14
                                        Layout.preferredHeight: 14
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredHeight: 8
                }
            }
        }
    }
}
