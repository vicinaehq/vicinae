import QtQuick

Item {
    id: root

    property bool clickable: false

    signal clicked

    readonly property int buttonSize: 26

    implicitWidth: clickable ? 20 : row.implicitWidth
    implicitHeight: Math.max(row.implicitHeight, clickable ? 26 : 0)

    Rectangle {
        visible: root.clickable && (mouseArea.containsMouse || launcher.footerMenuOpen)
        x: -Math.round((root.buttonSize - root.width) / 2)
        anchors.verticalCenter: parent.verticalCenter
        width: root.buttonSize
        height: root.buttonSize
        radius: 6
        color: Theme.listItemHoverBg
    }

    Row {
        id: row
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 6

        ViciImage {
            width: 20
            height: 20
            source: root.clickable ? Img.builtin("vicinae").withFillColor(Theme.textMuted) : launcher.navigationIcon
            visible: root.clickable || launcher.navigationIcon.valid
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

    MouseArea {
        id: mouseArea
        x: -Math.round((root.buttonSize - root.width) / 2)
        anchors.verticalCenter: parent.verticalCenter
        width: root.buttonSize
        height: root.buttonSize
        enabled: root.clickable
        hoverEnabled: enabled
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.clicked()
    }
}
