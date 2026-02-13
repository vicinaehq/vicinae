import QtQuick
import QtQuick.Controls

GenericListView {
    id: commandListView
    property var cmdModel: null
    model: cmdModel
    listModel: cmdModel

    emptyTitle: cmdModel && cmdModel.emptyTitle ? cmdModel.emptyTitle : "No results"
    emptyDescription: cmdModel ? cmdModel.emptyDescription : ""
    emptyIcon: {
        var _ = Theme.foreground
        var icon = cmdModel ? cmdModel.emptyIcon : ""
        return icon !== "" ? icon : "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
    }

    onItemSelected: function(index) { if (cmdModel) cmdModel.setSelectedIndex(index) }
    onItemActivated: function(index) { if (cmdModel) cmdModel.activateSelected() }

    Connections {
        target: cmdModel
        function onModelReset() {
            commandListView.selectFirst()
            // Force action panel recreation: if currentIndex didn't change
            // (same position after re-filter), onCurrentIndexChanged won't fire,
            // so we explicitly call setSelectedIndex.
            if (cmdModel) cmdModel.setSelectedIndex(commandListView.currentIndex)
        }
    }

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
                selected: commandListView.currentIndex === delegateLoader.index
                onClicked: commandListView.currentIndex = delegateLoader.index
                onDoubleClicked: commandListView.itemActivated(delegateLoader.index)
            }
        }
    }
}
