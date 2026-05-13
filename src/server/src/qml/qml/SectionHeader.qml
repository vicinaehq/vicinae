import QtQuick

Item {
    id: root
    height: 30

    required property string text
    property real leftPadding: 16

    Text {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        font.weight: Font.DemiBold
        elide: Text.ElideRight
    }
}
