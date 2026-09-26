pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Documents
import Vicinae.Scrolling as Scrolling

Item {
    id: root
    readonly property DocumentStyle style: root.model.style
    property Component imageDelegate: null
    property real topInset: 0
    property real bottomInset: 0
    DocumentScope.style: root.style
    signal linkActivated(string link)
    required property MarkdownModel model
    property int contentPadding: 12
    property int topPadding: contentPadding
    property string fontFamily: ""
    readonly property alias flickable: view
    readonly property alias contentHeight: view.contentHeight
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

    DocumentView {
        id: view
        anchors.fill: parent
        documentModel: root.model
        style: root.style
        topInset: root.topInset
        bottomInset: root.bottomInset
        layoutKey: [root.style.fontFamily, root.style.monoFontFamily, root.style.regularFontSize, root.style.smallerFontSize, root.fontFamily, root.contentPadding]
        topMargin: root.topPadding + root.topInset
        bottomMargin: root.contentPadding + root.bottomInset
        Scrolling.WheelHandler {
            target: view
        }
        ScrollBar.vertical: DocumentScrollBar {
            policy: ScrollBar.AsNeeded
            topPadding: root.topInset
            bottomPadding: root.bottomInset
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
                imageDelegate: root.imageDelegate
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
            root.linkActivated(link);
        }
    }
    ContextMenu.menu: DocumentSelectionMenu {
        controller: root.document
        onAboutToShow: view.forceActiveFocus()
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
