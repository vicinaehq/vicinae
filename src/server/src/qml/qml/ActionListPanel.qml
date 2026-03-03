import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root

    required property var model
    property var boundActions: root.model

    signal navigateBack()

    function moveUp() {
        var next = root.model.nextSelectableIndex(listView.currentIndex, -1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function moveDown() {
        var next = root.model.nextSelectableIndex(listView.currentIndex, 1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function moveSectionUp() {
        if (typeof root.model.nextSectionIndex !== "function") { moveUp(); return }
        var next = root.model.nextSectionIndex(listView.currentIndex, -1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function moveSectionDown() {
        if (typeof root.model.nextSectionIndex !== "function") { moveDown(); return }
        var next = root.model.nextSectionIndex(listView.currentIndex, 1)
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

        Rectangle {
            id: divider
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

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
                        const nav = launcher.matchNavigationKey(event.key, event.modifiers)
                        if (nav === 1) {
                            root.moveUp()
                            event.accepted = true
                        } else if (nav === 2) {
                            root.moveDown()
                            event.accepted = true
                        } else if (nav === 3) {
                            if (actionPanel.depth > 1) root.navigateBack()
                            event.accepted = true
                        } else if (event.key === Qt.Key_Backspace && filterInput.text === "" && actionPanel.depth > 1) {
                            root.navigateBack()
                            event.accepted = true
                        } else if ((event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.MetaModifier))
                                   && actionPanel.tryShortcut(event.key, event.modifiers)) {
                            event.accepted = true
                        }
                    }
                    Keys.onUpPressed: (event) => {
                        (event.modifiers & Qt.ControlModifier) ? root.moveSectionUp() : root.moveUp()
                    }
                    Keys.onDownPressed: (event) => {
                        (event.modifiers & Qt.ControlModifier) ? root.moveSectionDown() : root.moveDown()
                    }
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
