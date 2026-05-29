import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

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

    GenericListView {
        id: listView
        anchors.fill: parent

        listModel: root.host.listModel
        model: root.host.listModel
        autoWireModel: true
        detailComponent: detailPanel
        detailVisible: root.host.hasDetail

        emptyTitle: root.host.emptyTitle ?? "No results"
        emptyDescription: root.host.emptyDescription ?? ""
        emptyIcon: root.host.emptyIcon

        delegate: Loader {
            id: delegateLoader
            width: ListView.view.width

            required property int index
            required property bool isSection
            required property bool isSelectable
            required property string sectionName
            required property string title
            required property string subtitle
            required property string iconSource
            required property var itemAccessory

            sourceComponent: isSection ? sectionComponent : itemComponent

            Component {
                id: sectionComponent
                SectionHeader {
                    width: delegateLoader.width
                    text: delegateLoader.sectionName
                }
            }

            Component {
                id: itemComponent
                ListItemDelegate {
                    width: delegateLoader.width
                    itemTitle: delegateLoader.title
                    itemSubtitle: delegateLoader.subtitle
                    itemIconSource: delegateLoader.iconSource
                    itemAlias: ""
                    itemIsActive: false
                    itemAccessory: delegateLoader.itemAccessory
                    selected: listView.currentIndex === delegateLoader.index
                    onClicked: listView.currentIndex = delegateLoader.index
                    onActivated: listView.itemActivated(delegateLoader.index)
                }
            }
        }
    }

    readonly property bool _hasCustomDetail: root.host.detailContentUrl !== undefined && root.host.detailContentUrl.toString() !== ""

    Component {
        id: detailPanel

        DetailPanel {
            id: panel
            metadata: root.host.detailMetadata
            hasContent: root._hasCustomDetail || (root.host.detailContent !== undefined && root.host.detailContent !== "")

            Loader {
                anchors.fill: parent
                sourceComponent: root._hasCustomDetail ? null : (panel.hasContent ? defaultTextContent : null)
                source: root._hasCustomDetail ? root.host.detailContentUrl : ""
            }
        }
    }

    Component {
        id: defaultTextContent
        TextViewer {
            text: root.host.detailContent
        }
    }
}
