import QtQuick

Item {
    id: root

    required property string label
    required property var shortcutTokens
    property bool highlighted: false
    property bool backgrounded: false

    signal clicked

    readonly property bool hovered: mouseArea.containsMouse
    readonly property int horizontalPadding: 8
    readonly property int buttonHeight: 28

    implicitWidth: row.implicitWidth + 2 * horizontalPadding
    implicitHeight: buttonHeight

    SourceBlendRect {
        anchors.fill: parent
        visible: root.hovered || root.backgrounded
        radius: 6
        backgroundColor: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, Config.windowOpacity)
        color: Qt.rgba(Theme.listItemHoverBg.r, Theme.listItemHoverBg.g, Theme.listItemHoverBg.b, Config.windowOpacity)
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
