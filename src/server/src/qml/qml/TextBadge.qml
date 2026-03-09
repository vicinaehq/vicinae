import QtQuick

Item {
    id: root

    required property string text

    readonly property color _surfaceColor: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.96)
    readonly property color _borderColor: Qt.rgba(Theme.divider.r, Theme.divider.g, Theme.divider.b, 0.9)
    readonly property color _highlightColor: Qt.rgba(1, 1, 1, 0.08)
    readonly property color _shadowColor: Qt.rgba(0, 0, 0, 0.16)
    readonly property color _contentColor: Theme.textMuted

    implicitWidth: badgeBody.implicitWidth
    implicitHeight: badgeBody.implicitHeight
    width: implicitWidth
    height: implicitHeight

    Item {
        id: badgeBody
        implicitHeight: 22
        implicitWidth: badgeLabel.implicitWidth + 14
        anchors.fill: parent

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

        Text {
            id: badgeLabel
            anchors.centerIn: parent
            text: root.text
            color: root._contentColor
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize - 0.5
            maximumLineCount: 1
        }
    }
}
