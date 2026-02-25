import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // The backing model — must have Q_INVOKABLE nextSelectableIndex(int, int)
    required property var listModel

    property alias model: listView.model
    property alias delegate: listView.delegate
    property alias currentIndex: listView.currentIndex
    property alias count: listView.count

    // Optional detail panel — mirrors TypedListView's SplitDetailWidget pattern.
    // `detailComponent` is the template; `detailVisible` toggles per-item
    // (consumer sets it from selectionChanged, just like
    //  m_split->setDetailVisibility(generateDetail(item)) in TypedListView).
    property Component detailComponent: null
    property var detailProps: ({})
    property real detailRatio: 0.65
    property bool detailVisible: false

    property string emptyTitle: "No results"
    property string emptyDescription: ""
    property string emptyIcon: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
    property Component emptyViewComponent: null

    property bool suppressEmpty: false

    readonly property bool _showDetail: root.detailComponent !== null && root.detailVisible
    readonly property bool _empty: listView.count === 0
    readonly property bool _awaitingData: root.listModel && root.listModel.awaitingData === true

    // When true, GenericListView auto-wires common model signals:
    //  - onModelReset → selectFirst + setSelectedIndex
    //  - onItemSelected → setSelectedIndex
    //  - onItemActivated → activateSelected
    // Consumers only need to set listModel; no Connections block required.
    property bool autoWireModel: false

    // When false, onModelReset preserves the current selection index
    // instead of jumping to the first item. Useful for data refreshes
    // where the list content updates without the user changing the search text.
    property bool selectFirstOnReset: true

    signal itemActivated(int index)
    signal itemSelected(int index)

    function moveDown() {
        var next = root.listModel.nextSelectableIndex(listView.currentIndex, 1)
        if (next !== listView.currentIndex) {
            listView.currentIndex = next
            listView.positionViewAtIndex(next, ListView.Contain)
        }
    }

    function moveUp() {
        var next = root.listModel.nextSelectableIndex(listView.currentIndex, -1)
        if (next !== listView.currentIndex) {
            listView.currentIndex = next
            var scrollTarget = root.listModel.scrollTargetIndex(next, -1)
            listView.positionViewAtIndex(scrollTarget, ListView.Contain)
        }
    }

    function selectFirst() {
        listView.currentIndex = root.listModel.nextSelectableIndex(-1, 1)
    }

    onListModelChanged: {
        if (root.autoWireModel && root.listModel && listView.count > 0) {
            root.selectFirst()
            root.listModel.setSelectedIndex(listView.currentIndex)
        }
    }

    Connections {
        enabled: root.autoWireModel && root.listModel
        target: root.listModel
        function onModelReset() {
            if (root.selectFirstOnReset
                    || listView.currentIndex < 0
                    || listView.currentIndex >= listView.count) {
                root.selectFirst()
            }
            if (root.listModel) root.listModel.setSelectedIndex(listView.currentIndex)
        }
    }

    onItemSelected: function(index) {
        if (root.autoWireModel && root.listModel) root.listModel.setSelectedIndex(index)
    }
    onItemActivated: function(index) {
        if (root.autoWireModel && root.listModel) root.listModel.activateSelected()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        visible: !root._empty

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            reuseItems: true
            cacheBuffer: 200
            boundsBehavior: Flickable.StopAtBounds
            highlightMoveDuration: 0
            currentIndex: -1
            topMargin: 4
            bottomMargin: 4

            onCurrentIndexChanged: root.itemSelected(currentIndex)
            onCountChanged: {
                if (root.autoWireModel && root.listModel
                        && currentIndex < 0 && count > 0) {
                    root.selectFirst()
                    root.listModel.setSelectedIndex(currentIndex)
                }
            }

            ScrollBar.vertical: ViciScrollBar {
                policy: listView.contentHeight > listView.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }
        }

        Rectangle {
            visible: root._showDetail
            Layout.fillHeight: true
            implicitWidth: 1
            color: Theme.divider
        }

        Loader {
            id: detailLoader
            active: root._showDetail
            visible: active
            sourceComponent: root.detailComponent
            Layout.preferredWidth: root.width * root.detailRatio
            Layout.fillHeight: true
        }
    }

    Loader {
        anchors.fill: parent
        active: root._empty && !root.suppressEmpty && !root._awaitingData
        visible: active
        sourceComponent: root.emptyViewComponent
                         ? root.emptyViewComponent
                         : defaultEmptyView
    }

    Component {
        id: defaultEmptyView
        EmptyView {
            title: root.emptyTitle
            description: root.emptyDescription
            icon: root.emptyIcon
        }
    }
}
