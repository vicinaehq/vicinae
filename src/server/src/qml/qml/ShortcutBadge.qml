import QtQuick

Rectangle {
    property alias text: label.text

    width: label.implicitWidth + 10
    height: label.implicitHeight + 4
    radius: 4
    color: Theme.divider

    Text {
        id: label
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize - 1
        anchors.centerIn: parent
    }
}
