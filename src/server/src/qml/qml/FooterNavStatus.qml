import QtQuick

Item {
    id: root

    property bool clickable: false
    property bool rootMode: false
    property real availableWidth: 0

    signal clicked

    readonly property int buttonSize: 26
    readonly property int backgroundPadding: root.rootMode ? 0 : 6
    readonly property real textMaxWidth: Math.max(0, root.availableWidth - (navIcon.visible ? navIcon.width + row.spacing : 0))

    implicitWidth: root.rootMode ? 20 : row.implicitWidth
    implicitHeight: Math.max(row.implicitHeight, clickable ? 26 : 0)

    Rectangle {
        visible: root.clickable && (mouseArea.containsMouse || footerPanel.open)
        x: root.rootMode ? -Math.round((root.buttonSize - root.width) / 2) : -root.backgroundPadding
        anchors.verticalCenter: parent.verticalCenter
        width: root.rootMode ? root.buttonSize : root.width + 2 * root.backgroundPadding
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
            id: navIcon
            width: 20
            height: 20
            source: root.rootMode ? Img.builtin("vicinae").withFillColor(Theme.textMuted) : launcher.navigationIcon
            visible: root.rootMode || launcher.navigationIcon.valid
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: launcher.navigationTitle
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            elide: Text.ElideRight
            width: Math.min(implicitWidth, root.textMaxWidth)
            anchors.verticalCenter: parent.verticalCenter
            visible: launcher.navigationTitle !== ""
        }
    }

    MouseArea {
        id: mouseArea
        x: root.rootMode ? -Math.round((root.buttonSize - root.width) / 2) : -root.backgroundPadding
        anchors.verticalCenter: parent.verticalCenter
        width: root.rootMode ? root.buttonSize : root.width + 2 * root.backgroundPadding
        height: root.buttonSize
        enabled: root.clickable
        hoverEnabled: enabled
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: root.clicked()
    }
}
