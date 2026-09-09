pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property var tokens: []
    property color contentColor: Theme.foreground

    readonly property color _surfaceColor: Qt.rgba(root.contentColor.r, root.contentColor.g, root.contentColor.b, 0.08)
    readonly property color _borderColor: Qt.rgba(root.contentColor.r, root.contentColor.g, root.contentColor.b, 0.14)

    implicitWidth: tokenRow.implicitWidth
    implicitHeight: tokenRow.implicitHeight

    Row {
        id: tokenRow
        spacing: 4

        Repeater {
            model: root.tokens || []

            delegate: Item {
                id: tokenItem
                required property var modelData

                readonly property var token: modelData || ({})
                readonly property string tokenText: token["text"] || ""
                readonly property int tokenIcon: token["icon"] ?? -1
                readonly property bool compact: tokenIcon >= 0 || tokenText.length <= 2

                implicitHeight: 20
                implicitWidth: Math.max(compact ? implicitHeight : 0, tokenContent.implicitWidth + (compact ? 10 : 12))

                Rectangle {
                    anchors.fill: parent
                    radius: 6
                    color: root._surfaceColor
                    border.width: 1
                    border.color: root._borderColor
                }

                Item {
                    id: tokenContent
                    anchors.centerIn: parent
                    implicitWidth: tokenIconItem.visible ? tokenIconItem.width : tokenLabel.implicitWidth
                    implicitHeight: tokenIconItem.visible ? tokenIconItem.height : tokenLabel.implicitHeight

                    ViciImage {
                        id: tokenIconItem
                        visible: tokenItem.tokenIcon >= 0
                        source: visible ? Img.icon(tokenItem.tokenIcon).withFillColor(root.contentColor) : ""
                        width: 11
                        height: 11
                        anchors.centerIn: parent
                    }

                    Text {
                        id: tokenLabel
                        visible: tokenItem.tokenIcon < 0
                        text: tokenItem.tokenText
                        color: root.contentColor
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.smallerFontSize - 0.25
                        font.weight: Font.Medium
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
}
