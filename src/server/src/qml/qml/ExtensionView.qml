import QtQuick

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
        CommandListView {
            anchors.fill: parent
            cmdModel: root.host.contentModel
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
