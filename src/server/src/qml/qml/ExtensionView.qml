import QtQuick
import Vicinae

Item {
    id: root
    required property var host // QmlExtensionViewHost*

    // Navigation functions called by SearchBar via commandStack.currentItem
    function moveUp() {
        if (contentLoader.item && typeof contentLoader.item.moveUp === "function")
            contentLoader.item.moveUp()
    }
    function moveDown() {
        if (contentLoader.item && typeof contentLoader.item.moveDown === "function")
            contentLoader.item.moveDown()
    }
    function moveLeft() {
        if (contentLoader.item && typeof contentLoader.item.moveLeft === "function")
            contentLoader.item.moveLeft()
    }
    function moveRight() {
        if (contentLoader.item && typeof contentLoader.item.moveRight === "function")
            contentLoader.item.moveRight()
    }

    Loader {
        id: contentLoader
        anchors.fill: parent
        sourceComponent: {
            switch (root.host.viewType) {
            case "list": return listComponent
            case "grid": return gridComponent
            default: return null
            }
        }
    }

    Component {
        id: listComponent
        Item {
            function moveUp() { listView.moveUp() }
            function moveDown() { listView.moveDown() }

            GenericListView {
                id: listView
                anchors.fill: parent
                listModel: root.host.contentModel
                model: root.host.contentModel
                autoWireModel: true
                selectFirstOnReset: root.host.selectFirstOnReset

                emptyTitle: root.host.contentModel.emptyTitle || "No results"
                emptyDescription: root.host.contentModel.emptyDescription || ""
                emptyIcon: {
                    var _ = Theme.foreground
                    var icon = root.host.contentModel.emptyIcon || ""
                    return icon !== "" ? icon : "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground
                }

                detailComponent: detailPanel
                detailVisible: root.host.contentModel.isShowingDetail
                               && root.host.contentModel.hasDetail

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
                DetailPanel {
                    metadata: root.host.contentModel.detailMetadata

                    MarkdownText {
                        anchors.fill: parent
                        visible: root.host.contentModel.detailMarkdown !== ""
                        markdown: root.host.contentModel.detailMarkdown
                    }
                }
            }
        }
    }

    Component {
        id: gridComponent
        ExtensionGridView {
            anchors.fill: parent
            cmdModel: root.host.contentModel
        }
    }
}
