pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    property var tokens: []
    property color contentColor: Theme.foreground
    property bool keycaps: true
    property int keycapSize: 20

    property color surfaceColor: Qt.rgba(root.contentColor.r, root.contentColor.g, root.contentColor.b, 0.08)
    property color borderColor: Qt.rgba(root.contentColor.r, root.contentColor.g, root.contentColor.b, 0.14)

    implicitWidth: tokenRow.implicitWidth
    implicitHeight: tokenRow.implicitHeight

    Row {
        id: tokenRow
        spacing: root.keycaps ? 4 : 3

        Repeater {
            model: root.tokens || []

            delegate: Item {
                id: tokenItem
                required property var modelData

                readonly property var token: modelData || ({})
                readonly property string tokenText: token["text"] || ""
                readonly property int tokenIcon: token["icon"] ?? -1
                readonly property bool compact: tokenIcon >= 0 || tokenText.length <= 2

                implicitHeight: root.keycapSize
                implicitWidth: root.keycaps ? Math.max(compact ? implicitHeight : 0, tokenContent.implicitWidth + (compact ? 10 : 12)) : tokenContent.implicitWidth

                Rectangle {
                    visible: root.keycaps
                    anchors.fill: parent
                    radius: 6
                    color: root.surfaceColor
                    border.width: 1
                    border.color: root.borderColor
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
