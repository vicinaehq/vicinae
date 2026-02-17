import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    required property var model
    property int contentPadding: 12
    property alias contentHeight: flickable.contentHeight
    property var selectionController: null
    focus: true

    readonly property var _controller: selectionController ?? _internalController

    Keys.onUpPressed: flickable.flick(0, 800)
    Keys.onDownPressed: flickable.flick(0, -800)
    Keys.onPressed: event => {
        if (event.key === Qt.Key_PageUp) { flickable.flick(0, 2400); event.accepted = true }
        else if (event.key === Qt.Key_PageDown) { flickable.flick(0, -2400); event.accepted = true }
    }

    TextSelectionController {
        id: _internalController
        container: col
        flickable: flickable
        mdModel: root.model
    }

    // Cursor-only MouseArea below Flickable — sets I-beam for non-interactive gaps.
    // Interactive children (copy button) override with their own cursor.
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: Qt.IBeamCursor
    }

    // The Flickable handles wheel scrolling natively (interactive: true).
    // Press/move/release are intercepted by the TextSelectionController's
    // event filter for text selection — they never reach the Flickable's
    // own flick handling. Interactive children (e.g. copy button) receive
    // events directly from the scene graph, bypassing the filter.
    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: col.implicitHeight + root.contentPadding * 2
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        ColumnLayout {
            id: col
            x: root.contentPadding
            y: root.contentPadding
            width: flickable.width - root.contentPadding * 2
            spacing: 8

            Repeater {
                model: root.model

                Loader {
                    id: blockLoader
                    Layout.fillWidth: true

                    required property int index
                    required property int blockType
                    required property var blockData

                    // MdBlockType enum values from C++
                    readonly property int heading: 0
                    readonly property int paragraph: 1
                    readonly property int codeBlock: 2
                    readonly property int bulletList: 3
                    readonly property int orderedList: 4
                    readonly property int table: 5
                    readonly property int image: 6
                    readonly property int horizontalRule: 7
                    readonly property int htmlBlock: 8
                    readonly property int blockquote: 9
                    readonly property int callout: 10

                    sourceComponent: {
                        switch (blockType) {
                        case heading:        return headingComp
                        case paragraph:      return paragraphComp
                        case codeBlock:      return codeBlockComp
                        case bulletList:     return listComp
                        case orderedList:    return listComp
                        case table:          return tableComp
                        case image:          return imageComp
                        case horizontalRule: return hrComp
                        case htmlBlock:      return htmlBlockComp
                        case blockquote:     return blockquoteComp
                        case callout:        return calloutComp
                        default:             return null
                        }
                    }

                    onLoaded: {
                        item.selectionController = root._controller
                        item.mdModel = root.model
                        item.blockIndex = index
                        if (blockType === orderedList)
                            item.ordered = true
                        else if (blockType === bulletList)
                            item.ordered = false
                        // Set blockData last — it triggers Repeater creation in
                        // multi-TextEdit blocks, and children need selectionController
                        // to already be set for registration in Component.onCompleted
                        item.blockData = blockData
                    }
                }
            }
        }

        Component { id: headingComp;    MdHeading {} }
        Component { id: paragraphComp;  MdParagraph {} }
        Component { id: codeBlockComp;  MdCodeBlock {} }
        Component { id: listComp;       MdList {} }
        Component { id: tableComp;      MdTable {} }
        Component { id: imageComp;      MdImage {} }
        Component { id: hrComp;         MdHorizontalRule {} }
        Component { id: htmlBlockComp;  MdHtmlBlock {} }
        Component { id: blockquoteComp; MdBlockquote {} }
        Component { id: calloutComp;    MdCallout {} }
    }

    Shortcut {
        sequence: StandardKey.Copy
        enabled: root._controller.hasSelection
        onActivated: root._controller.copy()
    }

    Shortcut {
        sequence: StandardKey.SelectAll
        onActivated: root._controller.selectAll()
    }

    Connections {
        target: root.model
        function onModelReset() { root._controller.clearSelection() }
    }
}
