pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root
    required property ExtensionViewHost host

    StackView.onActivated: {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.restoreFocus === "function")
            contentLoader.item.restoreFocus();
        // qmllint enable missing-property
    }

    function moveUp() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveUp === "function") {
            return contentLoader.item.moveUp();
        // qmllint enable missing-property
        }
        return false;
    }
    function moveDown() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveDown === "function") {
            return contentLoader.item.moveDown();
        // qmllint enable missing-property
        }
        return false;
    }
    function moveSectionUp() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveSectionUp === "function") {
            return contentLoader.item.moveSectionUp();
        // qmllint enable missing-property
        } else {
            return moveUp();
        }
    }
    function moveSectionDown() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveSectionDown === "function")
            return contentLoader.item.moveSectionDown();
        // qmllint enable missing-property
        else
            return moveDown();
    }
    function moveLeft() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveLeft === "function") {
            return contentLoader.item.moveLeft();
        // qmllint enable missing-property
        }
        return false;
    }
    function moveRight() {
        // qmllint disable missing-property
        if (contentLoader.item && typeof contentLoader.item.moveRight === "function") {
            return contentLoader.item.moveRight();
        // qmllint enable missing-property
        }
        return false;
    }

    Loader {
        id: contentLoader
        anchors.fill: parent
        sourceComponent: {
            switch (root.host.viewType) {
            case "list":
                return listComponent;
            case "grid":
                return gridComponent;
            case "detail":
                return detailViewComponent;
            case "form":
                return formComponent;
            default:
                return null;
            }
        }
    }

    Component {
        id: listComponent
        Item {
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
                readonly property ExtensionListModel extModel: root.host.contentModel as ExtensionListModel
                anchors.fill: parent
                listModel: extModel
                model: root.host.contentModel
                canLoadMore: root.host.hasMorePages
                onEndReached: root.host.onLoadMore()

                autoWireModel: true
                selectFirstOnReset: root.host.selectFirstOnReset
                suppressEmpty: root.host.suppressEmptyView

                emptyTitle: listView.extModel?.emptyTitle || qsTr("No results")
                emptyDescription: listView.extModel?.emptyDescription || ""
                emptyIcon: listView.extModel?.emptyIcon.valid ? listView.extModel.emptyIcon : Img.builtin("magnifying-glass").withFillColor(Theme.foreground)

                detailComponent: detailPanel
                detailVisible: listView.extModel?.isShowingDetail ?? false

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
                            selected: listView.currentIndex === delegateLoader.index
                            draggable: delegateLoader.isDraggable
                            onClicked: listView.currentIndex = delegateLoader.index
                            onActivated: listView.itemActivated(delegateLoader.index)
                            onDragRequested: function (source) {
                                listView.currentIndex = delegateLoader.index;
                                listView.extModel?.startDrag(delegateLoader.index, source);
                            }
                        }
                    }
                }
            }

            Component {
                id: detailPanel
                DetailPanel {
                    hasContent: (listView.extModel?.detailMarkdown ?? "") !== ""
                    metadata: listView.extModel?.detailMetadata ?? []

                    MarkdownText {
                        anchors.fill: parent
                        topPadding: 6
                        markdown: listView.extModel?.detailMarkdown ?? ""
                    }
                }
            }
        }
    }

    Component {
        id: gridComponent
        ExtensionGridView {
            anchors.fill: parent
            cmdModel: root.host.contentModel as SectionGridModel
            suppressEmpty: root.host.suppressEmptyView
            canLoadMore: root.host.hasMorePages
            onEndReached: root.host.onLoadMore()
        }
    }

    Component {
        id: detailViewComponent
        MarkdownDetailView {
            anchors.fill: parent
            markdown: root.host.detailMarkdown
            metadata: root.host.detailMetadata
        }
    }

    Component {
        id: formComponent
        ExtensionFormView {
            anchors.fill: parent
            host: root.host
            formModel: root.host.contentModel as ExtensionFormModel
        }
    }
}
