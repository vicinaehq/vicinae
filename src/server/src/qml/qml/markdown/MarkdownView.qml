import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property var model
    property int contentPadding: 12
    property alias contentHeight: flickable.contentHeight

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: col.implicitHeight + root.contentPadding * 2
        clip: true
        interactive: false
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
            policy: flickable.contentHeight > flickable.height
                    ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
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

                    // Pass data to loaded component via properties
                    onLoaded: {
                        item.blockData = blockData
                        item.mdModel = root.model
                        item.blockIndex = index
                        if (blockType === orderedList)
                            item.ordered = true
                        else if (blockType === bulletList)
                            item.ordered = false
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

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: wheel => {
            var maxY = Math.max(0, flickable.contentHeight - flickable.height)
            var dy = wheel.pixelDelta.y !== 0 ? -wheel.pixelDelta.y
                                              : (wheel.angleDelta.y > 0 ? -40 : 40)
            flickable.contentY = Math.max(0, Math.min(flickable.contentY + dy, maxY))
        }
    }
}
