import QtQuick
import QtQuick.Controls

GenericListView {
    id: commandListView
    property var cmdModel: null
    model: cmdModel
    listModel: cmdModel
    autoWireModel: true
    selectFirstOnReset: cmdModel ? cmdModel.selectFirstOnReset : true

    emptyTitle: cmdModel && cmdModel.emptyTitle || qsTr("No results")
    emptyDescription: (cmdModel && cmdModel.emptyDescription) || ""
    emptyIcon: cmdModel?.emptyIcon?.valid ? cmdModel.emptyIcon : Img.builtin("magnifying-glass").withFillColor(Theme.foreground)

    delegate: Loader {
        id: delegateLoader
        width: ListView.view.width

        required property int index
        required property bool isSection
        required property bool isSelectable
        required property int quickAccessIndex
        required property string sectionName
        required property string title
        required property string subtitle
        required property string iconSource
        required property var itemAccessory
        required property bool isDraggable

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
                quickAccessIndex: delegateLoader.quickAccessIndex
                selected: commandListView.currentIndex === delegateLoader.index
                draggable: delegateLoader.isDraggable
                onClicked: commandListView.currentIndex = delegateLoader.index
                onActivated: commandListView.itemActivated(delegateLoader.index)
                onDragRequested: function (source) {
                    commandListView.currentIndex = delegateLoader.index;
                    commandListView.cmdModel.startDrag(delegateLoader.index, source);
                }
            }
        }
    }
}
