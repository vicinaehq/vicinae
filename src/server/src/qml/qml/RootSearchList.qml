import QtQuick
import QtQuick.Controls

GenericListView {
    id: searchListView
    model: searchModel
    listModel: searchModel

    onItemSelected: function(index) { searchModel.setSelectedIndex(index) }
    onItemActivated: function(index) { searchModel.activateSelected() }

    Connections {
        target: searchModel
        function onModelReset() {
            searchListView.selectFirst()
            // currentIndex may not change after model reset (same first-selectable
            // position), so onCurrentIndexChanged won't fire and setSelectedIndex
            // won't be called.  Force it explicitly to recreate the action panel.
            searchModel.setSelectedIndex(searchListView.currentIndex)
        }
    }

    delegate: Loader {
        id: delegateLoader
        width: ListView.view.width

        required property int index
        required property bool isSection
        required property bool isSelectable
        required property string sectionName
        required property string itemType
        required property string title
        required property string subtitle
        required property string iconSource
        required property string alias
        required property bool isActive

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
                itemAlias: delegateLoader.alias
                itemIsActive: delegateLoader.isActive
                selected: searchListView.currentIndex === delegateLoader.index
                onClicked: searchListView.currentIndex = delegateLoader.index
                onDoubleClicked: searchListView.itemActivated(delegateLoader.index)
            }
        }
    }
}
