pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property BrowserBookmarksViewHost host

    function moveUp() {
        return listView.moveUp();
    }
    function moveDown() {
        return listView.moveDown();
    }
    function moveSectionUp() {
        return listView.moveSectionUp();
    }
    function moveSectionDown() {
        return listView.moveSectionDown();
    }

    CommandListView {
        id: listView
        anchors.fill: parent

        cmdModel: root.host.listModel
        emptyTitle: root.host.emptyTitle || qsTr("No results")
        emptyDescription: root.host.emptyDescription
        emptyIcon: root.host.emptyIcon.valid ? root.host.emptyIcon : Img.icon(BuiltinIcon.MagnifyingGlass).withFillColor(Theme.foreground)
        detailComponent: detailPanel
        detailVisible: root.host.detailVisible
    }

    Component {
        id: detailPanel

        DetailPanel {
            hasContent: false
            metadata: root.host.detailMetadata
        }
    }
}
