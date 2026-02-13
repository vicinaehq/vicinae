import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // The backing model — must have Q_INVOKABLE nextSelectableIndex(int, int)
    required property var listModel

    // Forwarded to the internal ListView
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
    property real detailRatio: 0.45
    property bool detailVisible: false

    readonly property bool _showDetail: root.detailComponent !== null && root.detailVisible

    signal itemActivated(int index)
    signal itemSelected(int index)

    function moveDown() {
        var next = root.listModel.nextSelectableIndex(listView.currentIndex, 1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function moveUp() {
        var next = root.listModel.nextSelectableIndex(listView.currentIndex, -1)
        if (next !== listView.currentIndex) listView.currentIndex = next
    }

    function selectFirst() {
        listView.currentIndex = root.listModel.nextSelectableIndex(-1, 1)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

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

            onCurrentIndexChanged: root.itemSelected(currentIndex)

            ScrollBar.vertical: ScrollBar {
                policy: listView.contentHeight > listView.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }
        }

        // Vertical divider — only when detail is visible
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
}
