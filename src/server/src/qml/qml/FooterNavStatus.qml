import QtQuick

Row {
    spacing: 6

    ViciImage {
        width: 20
        height: 20
        source: launcher.navigationIcon
        visible: launcher.navigationIcon.valid
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: launcher.navigationTitle
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
        anchors.verticalCenter: parent.verticalCenter
        visible: launcher.navigationTitle !== ""
    }
}
