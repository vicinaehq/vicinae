import QtQuick

Item {
    id: root

    required property string label
    required property var shortcutTokens
    property bool highlighted: false

    signal clicked

    readonly property bool hovered: mouseArea.containsMouse
    readonly property int horizontalPadding: 8
    readonly property int buttonHeight: 28

    implicitWidth: row.implicitWidth + 2 * horizontalPadding
    implicitHeight: buttonHeight

    Rectangle {
        anchors.fill: parent
        visible: root.hovered
        radius: 6
        color: Theme.listItemHoverBg
    }

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 6

        Text {
            text: root.label
            color: root.highlighted ? Theme.foreground : Theme.textMuted
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            anchors.verticalCenter: parent.verticalCenter
        }

        ShortcutBadge {
            visible: root.shortcutTokens && root.shortcutTokens.length > 0
            tokens: root.shortcutTokens
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
