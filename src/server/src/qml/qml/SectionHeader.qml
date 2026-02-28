import QtQuick

Item {
    id: root
    height: 30

    required property string text

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        font.weight: Font.DemiBold
    }
}
