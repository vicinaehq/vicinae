pragma ComponentBehavior: Bound
import QtQuick
import Vicinae.Documents

Loader {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style
    required property int blockType
    required property var blockData
    property int blockIndex: -1
    property MarkdownModel mdModel: null
    property Component imageDelegate: null
    property string fontFamily: ""
    property real fontSize: root.style.regularFontSize
    property real lineHeight: 1.0
    property real maxImageHeight: 200

    sourceComponent: {
        switch (root.blockType) {
        case Markdown.Heading:
            return headingComp;
        case Markdown.Paragraph:
            return paragraphComp;
        case Markdown.Math:
            return mathComp;
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
            fontSize: root.fontSize
            lineHeight: root.lineHeight
            first: root.blockIndex === 0
        }
    }
    Component {
        id: paragraphComp
        MdParagraph {
            blockData: root.blockData
            fontFamily: root.fontFamily
            fontSize: root.fontSize
            lineHeight: root.lineHeight
        }
    }
    Component {
        id: mathComp
        MdMath {
            blockData: root.blockData
            fontSize: root.fontSize
        }
    }
    Component {
        id: codeBlockComp
        MdCodeBlock {
            fontSize: root.fontSize * 0.9
            lineHeight: root.lineHeight
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
            fontSize: root.fontSize
            lineHeight: root.lineHeight
            ordered: root.blockType === Markdown.OrderedList
        }
    }
    Component {
        id: tableComp
        MdTable {
            blockData: root.blockData
            fontFamily: root.fontFamily
            fontSize: root.fontSize
            lineHeight: root.lineHeight
        }
    }
    Component {
        id: imageComp
        MdImage {
            fontSize: root.fontSize * 0.9
            blockData: root.blockData
            maxImageHeight: root.maxImageHeight
            imageDelegate: root.imageDelegate
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
            fontSize: root.fontSize
            lineHeight: root.lineHeight
        }
    }
    Component {
        id: blockquoteComp
        MdBlockquote {
            blockData: root.blockData
            fontFamily: root.fontFamily
            fontSize: root.fontSize
            lineHeight: root.lineHeight
        }
    }
    Component {
        id: calloutComp
        MdCallout {
            blockData: root.blockData
            fontFamily: root.fontFamily
            fontSize: root.fontSize
            lineHeight: root.lineHeight
        }
    }
}
