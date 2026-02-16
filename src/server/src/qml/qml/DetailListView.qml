import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

    function moveUp() { listView.moveUp() }
    function moveDown() { listView.moveDown() }

    GenericListView {
        id: listView
        anchors.fill: parent

        listModel: root.host.listModel
        model: root.host.listModel
        autoWireModel: true
        detailComponent: detailPanel
        detailVisible: root.host.hasDetail
        detailRatio: 0.65

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
            required property string itemAccessory

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
                    onDoubleClicked: listView.itemActivated(delegateLoader.index)
                }
            }
        }
    }

    readonly property bool _hasCustomDetail: root.host.detailContentUrl !== undefined
                                              && root.host.detailContentUrl.toString() !== ""

    Component {
        id: detailPanel

        DetailPanel {
            metadata: root.host.detailMetadata

            Loader {
                anchors.fill: parent
                sourceComponent: root._hasCustomDetail ? null : defaultTextContent
                source: root._hasCustomDetail ? root.host.detailContentUrl : ""
            }
        }
    }

    Component {
        id: defaultTextContent
        ScrollView {
            clip: true
            Text {
                width: parent.width
                text: root.host.detailContent
                color: Theme.foreground
                font.pointSize: Theme.smallerFontSize
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                padding: 12
            }
        }
    }
}
