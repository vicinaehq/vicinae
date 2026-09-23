pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Loader {
    id: root
    required property int blockType
    required property var blockData
    property int blockIndex: -1
    property var mdModel: null
    property string fontFamily: ""
    property real maxImageHeight: 200

    sourceComponent: {
        switch (root.blockType) {
        case Markdown.Heading:
            return headingComp;
        case Markdown.Paragraph:
            return paragraphComp;
        case Markdown.CodeBlock:
            return codeBlockComp;
        case Markdown.BulletList:
            return listComp;
        case Markdown.OrderedList:
            return listComp;
        case Markdown.Table:
            return tableComp;
        case Markdown.Image:
            return imageComp;
        case Markdown.HorizontalRule:
            return hrComp;
        case Markdown.HtmlBlock:
            return htmlBlockComp;
        case Markdown.Blockquote:
            return blockquoteComp;
        case Markdown.Callout:
            return calloutComp;
        default:
            return null;
        }
    }

    Component {
        id: headingComp
        MdHeading {
            blockData: root.blockData
            fontFamily: root.fontFamily
            first: root.blockIndex === 0
        }
    }
    Component {
        id: paragraphComp
        MdParagraph {
            blockData: root.blockData
            fontFamily: root.fontFamily
        }
    }
    Component {
        id: codeBlockComp
        MdCodeBlock {
            blockData: root.blockData
            blockIndex: root.blockIndex
            mdModel: root.mdModel
        }
    }
    Component {
        id: listComp
        MdList {
            blockData: root.blockData
            fontFamily: root.fontFamily
            ordered: root.blockType === Markdown.OrderedList
        }
    }
    Component {
        id: tableComp
        MdTable {
            blockData: root.blockData
            fontFamily: root.fontFamily
        }
    }
    Component {
        id: imageComp
        MdImage {
            blockData: root.blockData
            maxImageHeight: root.maxImageHeight
        }
    }
    Component {
        id: hrComp
        MdHorizontalRule {
            blockData: root.blockData
        }
    }
    Component {
        id: htmlBlockComp
        MdHtmlBlock {
            blockData: root.blockData
            fontFamily: root.fontFamily
        }
    }
    Component {
        id: blockquoteComp
        MdBlockquote {
            blockData: root.blockData
            fontFamily: root.fontFamily
        }
    }
    Component {
        id: calloutComp
        MdCallout {
            blockData: root.blockData
            fontFamily: root.fontFamily
        }
    }
}
