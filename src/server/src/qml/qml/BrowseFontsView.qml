import QtQuick
import QtQuick.Layouts
import Vicinae

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
        detailVisible: root.host.selectedFont !== ""
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

    Component {
        id: detailPanel

        MarkdownText {
            anchors.fill: parent
            markdown: root.host.showcaseMarkdown
            fontFamily: root.host.selectedFont
        }
    }
}
