import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    required property var model
    property var boundActions: root.model

    signal navigateBack()

    // Expose navigation functions for parent popover
    function moveUp() {
        var next = root.model.nextSelectableIndex(listView.currentIndex, -1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function moveDown() {
        var next = root.model.nextSelectableIndex(listView.currentIndex, 1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function activateCurrent() {
        if (listView.currentIndex >= 0)
            root.model.activate(listView.currentIndex)
    }

    function focusFilter() {
        filterInput.forceActiveFocus()
    }

    readonly property int listPadding: 6

    readonly property bool _empty: listView.count === 0

    readonly property int emptyPadding: 32

    implicitHeight: (_empty ? emptyLabel.implicitHeight + 2 * emptyPadding : listView.contentHeight + listView.topMargin + listView.bottomMargin)
                    + filterBar.height + divider.height

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Empty state — shown when filter produces no results
        Text {
            id: emptyLabel
            visible: root._empty
            text: "No matching actions"
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: root.emptyPadding
            Layout.bottomMargin: root.emptyPadding
        }

        ListView {
            id: listView
            visible: !root._empty
            Layout.fillWidth: true
            Layout.fillHeight: true
            topMargin: root.listPadding
            bottomMargin: root.listPadding
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            highlightMoveDuration: 0
            model: root.model

            delegate: Loader {
                id: delegateLoader
                width: listView.width
                required property int index
                required property string itemType
                required property string title
                required property string iconSource
                required property string shortcutLabel
                required property bool isSubmenu
                required property bool isPrimary
                required property bool isDanger

                sourceComponent: {
                    if (itemType === "section") return sectionComponent
                    if (itemType === "divider") return dividerComponent
                    return actionComponent
                }

                Component {
                    id: sectionComponent
                    SectionHeader {
                        width: delegateLoader.width
                        text: delegateLoader.title
                    }
                }

                Component {
                    id: dividerComponent
                    Item {
                        width: delegateLoader.width
                        height: 9
                        Rectangle {
                            anchors.centerIn: parent
                            width: parent.width - 24
                            height: 1
                            color: Theme.divider
                        }
                    }
                }

                Component {
                    id: actionComponent
                    ActionItemDelegate {
                        width: delegateLoader.width
                        title: delegateLoader.title
                        iconSource: delegateLoader.iconSource
                        shortcutLabel: delegateLoader.shortcutLabel
                        isSubmenu: delegateLoader.isSubmenu
                        isDanger: delegateLoader.isDanger
                        selected: listView.currentIndex === delegateLoader.index

                        onClicked: {
                            listView.currentIndex = delegateLoader.index
                            root.model.activate(delegateLoader.index)
                        }
                    }
                }
            }

            ScrollBar.vertical: ViciScrollBar {
                policy: listView.contentHeight > listView.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }
        }

        // Divider above filter
        Rectangle {
            id: divider
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

        // Filter input bar
        Item {
            id: filterBar
            Layout.fillWidth: true
            Layout.preferredHeight: 36

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Image {
                    source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
                    sourceSize.width: 14
                    sourceSize.height: 14
                    Layout.preferredWidth: 14
                    Layout.preferredHeight: 14
                    Layout.alignment: Qt.AlignVCenter
                    opacity: 0.5
                }

                TextInput {
                    id: filterInput
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: TextInput.AlignVCenter
                    font.pointSize: Theme.smallerFontSize
                    color: Theme.foreground
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                    clip: true

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        text: "Filter actions..."
                        color: Theme.textPlaceholder
                        font: filterInput.font
                        visible: !filterInput.text
                    }

                    onTextEdited: root.model.setFilter(text)

                    Keys.onPressed: function(event) {
                        if (event.key === Qt.Key_Backspace && filterInput.text === "" && actionPanel.depth > 1) {
                            root.navigateBack()
                            event.accepted = true
                        } else if ((event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.MetaModifier))
                                   && actionPanel.tryShortcut(event.key, event.modifiers)) {
                            event.accepted = true
                        }
                    }
                    Keys.onUpPressed: root.moveUp()
                    Keys.onDownPressed: root.moveDown()
                    Keys.onReturnPressed: root.activateCurrent()
                }
            }
        }
    }

    // Auto-select first selectable item on creation and after filter changes
    Connections {
        target: root.model
        function onModelReset() {
            var first = root.model.nextSelectableIndex(-1, 1)
            listView.currentIndex = first >= 0 ? first : -1
        }
    }

    Component.onCompleted: {
        if (root.model) {
            var first = root.model.nextSelectableIndex(-1, 1)
            if (first >= 0) listView.currentIndex = first
        }
    }
}
