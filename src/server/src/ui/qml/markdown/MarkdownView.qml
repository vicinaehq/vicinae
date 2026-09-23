pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Item {
    id: root
    required property MarkdownModel model
    property int contentPadding: 12
    property int topPadding: contentPadding
    property string fontFamily: ""
    property alias contentHeight: view.contentHeight
    readonly property alias document: view.document
    property bool _autoScroll: false
    focus: true

    function scrollUp() {
        view.flick(0, 800);
    }
    function scrollDown() {
        view.flick(0, -800);
    }
    Keys.onUpPressed: scrollUp()
    Keys.onDownPressed: scrollDown()
    Keys.onPressed: event => {
        if (event.key === Qt.Key_PageUp)
            view.flick(0, 2400);
        else if (event.key === Qt.Key_PageDown)
            view.flick(0, -2400);
        else
            event.accepted = false;
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.IBeamCursor
    }

    ScrollViewport {
        anchors.fill: parent
        flickable: view
        topPadding: root.topPadding
        bottomPadding: root.contentPadding
        DocumentView {
            id: view
            anchors.fill: parent
            documentModel: root.model
            layoutKey: [Theme.fontFamily, Theme.monoFontFamily, Theme.regularFontSize, Theme.smallerFontSize, root.fontFamily, root.contentPadding]
            topMargin: root.topPadding
            bottomMargin: root.contentPadding
            ViciWheelHandler {
                target: view
            }
            ScrollBar.vertical: ViciScrollBar {
                policy: ScrollBar.AsNeeded
            }
            delegate: Item {
                id: blockDelegate
                required property int index
                required property int blockType
                required property var blockData
                DocumentScope.row: index
                width: view.width
                height: block.implicitHeight
                MarkdownBlock {
                    id: block
                    x: root.contentPadding
                    width: parent.width - root.contentPadding * 2
                    blockType: blockDelegate.blockType
                    blockData: blockDelegate.blockData
                    blockIndex: blockDelegate.index
                    mdModel: root.model
                    fontFamily: root.fontFamily
                    maxImageHeight: root.height * 0.7
                }
            }
            onContentHeightChanged: {
                if (root._autoScroll)
                    Qt.callLater(() => {
                        view.scrollToEnd();
                        root._autoScroll = false;
                    });
            }
        }
    }
    Shortcut {
        sequences: [StandardKey.Copy]
        enabled: (root.activeFocus || view.activeFocus) && root.document.hasSelection
        onActivated: root.document.copy()
    }
    Shortcut {
        sequences: [StandardKey.SelectAll]
        enabled: root.activeFocus || view.activeFocus
        onActivated: root.document.selectAll()
    }
    Connections {
        target: view.document
        function onLinkActivated(link) {
            root.model.openLink(link);
        }
    }
    DocumentSelectionMenu {
        id: selectionMenu
        controller: root.document
    }
    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: eventPoint => {
            view.forceActiveFocus();
            selectionMenu.x = eventPoint.position.x;
            selectionMenu.y = eventPoint.position.y;
            selectionMenu.open();
        }
    }
    Connections {
        target: root.model
        function onBlocksAppended() {
            root._autoScroll = view.atYEnd && !view.document.hasSelection;
        }
        function onModelReset() {
            view.scrollToBeginning();
            root._autoScroll = false;
        }
    }
}
