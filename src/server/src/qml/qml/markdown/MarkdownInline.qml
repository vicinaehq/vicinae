import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property string markdown: ""
    property string fontFamily: ""
    implicitHeight: col.implicitHeight

    MarkdownModel {
        id: mdModel
    }

    ColumnLayout {
        id: col
        width: root.width
        spacing: 8

        Repeater {
            model: mdModel

            Loader {
                id: blockLoader
                Layout.fillWidth: true

                required property int index
                required property int blockType
                required property var blockData

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
                    item.selectionController = null
                    item.mdModel = mdModel
                    item.blockIndex = index
                    if (blockType !== codeBlock && blockType !== image && blockType !== horizontalRule)
                        item.fontFamily = Qt.binding(() => root.fontFamily)
                    if (blockType === orderedList)
                        item.ordered = true
                    else if (blockType === bulletList)
                        item.ordered = false
                    if (blockType === image)
                        item.maxImageHeight = Qt.binding(() => root.height * 0.7)
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

    onMarkdownChanged: mdModel.setMarkdown(markdown)
    Component.onCompleted: {
        if (markdown.length > 0)
            mdModel.setMarkdown(markdown)
    }
}
