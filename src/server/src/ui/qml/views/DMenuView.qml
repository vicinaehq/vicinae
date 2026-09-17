pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    required property DMenuViewHost host

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

    Component {
        id: detailPanel

        DetailPanel {
            metadata: [
                {
                    label: qsTr("Name"),
                    value: root.host.detailName
                },
                {
                    label: qsTr("Path"),
                    value: root.host.detailPath
                },
                {
                    label: qsTr("Type"),
                    value: root.host.detailMimeType
                }
            ]

            FilePreview {
                anchors.fill: parent
                imageSource: root.host.detailImageSource
                textContent: root.host.detailTextContent
                mimeType: root.host.detailMimeType
            }
        }
    }
}
