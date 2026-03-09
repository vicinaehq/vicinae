import QtQuick

Item {
    id: root

    property var tokens: []

    readonly property color _surfaceColor: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.96)
    readonly property color _borderColor: Qt.rgba(Theme.divider.r, Theme.divider.g, Theme.divider.b, 0.9)
    readonly property color _highlightColor: Qt.rgba(1, 1, 1, 0.08)
    readonly property color _shadowColor: Qt.rgba(0, 0, 0, 0.16)
    readonly property color _contentColor: Theme.textMuted

    implicitWidth: tokenRow.implicitWidth
    implicitHeight: tokenRow.implicitHeight

    Row {
        id: tokenRow
        spacing: 3

        Repeater {
            model: root.tokens || []

            delegate: Item {
                required property var modelData

                readonly property var token: modelData || ({})
                readonly property string tokenText: token["text"] || ""
                readonly property string tokenIcon: token["icon"] || ""
                readonly property bool compact: tokenIcon !== "" || tokenText.length <= 2

                implicitHeight: 22
                implicitWidth: Math.max(compact ? implicitHeight : 0, tokenContent.implicitWidth + (compact ? 12 : 14))

                Rectangle {
                    anchors.fill: parent
                    radius: 7
                    color: root._surfaceColor
                    border.width: 1
                    border.color: root._borderColor
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.topMargin: 1
                    anchors.leftMargin: 1
                    anchors.rightMargin: 1
                    height: Math.max(1, Math.round(parent.height * 0.42))
                    radius: 6
                    color: root._highlightColor
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 1
                    anchors.rightMargin: 1
                    anchors.bottomMargin: 1
                    height: 2
                    radius: 6
                    color: root._shadowColor
                }

                Item {
                    id: tokenContent
                    anchors.centerIn: parent
                    implicitWidth: tokenIconItem.visible ? tokenIconItem.width : tokenLabel.implicitWidth
                    implicitHeight: tokenIconItem.visible ? tokenIconItem.height : tokenLabel.implicitHeight

                    ViciImage {
                        id: tokenIconItem
                        visible: tokenIcon !== ""
                        source: visible ? Img.builtin(tokenIcon).withFillColor(root._contentColor) : ""
                        width: 12
                        height: 12
                        anchors.centerIn: parent
                    }

                    Text {
                        id: tokenLabel
                        visible: tokenIcon === ""
                        text: tokenText
                        color: root._contentColor
                        font.family: Theme.fontFamily
                        font.pointSize: Theme.smallerFontSize - 0.5
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
}
