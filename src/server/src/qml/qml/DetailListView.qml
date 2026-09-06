import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    required property var host

    function moveUp() {
        listView.moveUp();
    }
    function moveDown() {
        listView.moveDown();
    }
    function moveSectionUp() {
        listView.moveSectionUp();
    }
    function moveSectionDown() {
        listView.moveSectionDown();
    }

    GenericListView {
        id: listView
        anchors.fill: parent

        listModel: root.host.listModel
        model: root.host.listModel
        autoWireModel: true
        detailComponent: detailPanel
        detailVisible: root.host.hasDetail

        emptyTitle: root.host.emptyTitle ?? qsTr("No results")
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

    // Hosts exposing `detailMarkdown` get their detail rendered as markdown instead of plain text.
    readonly property bool _hasMarkdownDetail: root.host.detailMarkdown !== undefined

    Component {
        id: detailPanel

        DetailPanel {
            metadata: root.host.detailMetadata

            Loader {
                anchors.fill: parent
                sourceComponent: root._hasCustomDetail ? null : (root._hasMarkdownDetail ? markdownContent : defaultTextContent)
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

    Component {
        id: markdownContent
        MarkdownText {
            markdown: root.host.detailMarkdown
            topPadding: 6
        }
    }
}
