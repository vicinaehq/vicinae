import QtQuick

Rectangle {
    id: root

    property string text: ""
    property string icon: ""
    property var iconSource
    property string variant: "ghost"
    property bool bordered: false
    property color foreground: root.variant === "accent" ? Theme.listItemSelectionFg : Theme.foreground
    property real iconSize: 16
    property real horizontalPadding: 12
    property bool showFocus: activeFocus

    signal clicked

    readonly property bool hovered: mouseArea.containsMouse
    readonly property bool _hasIcon: root.icon !== "" || root.iconSource !== undefined
    readonly property bool _hasText: root.text !== ""

    implicitWidth: root._hasIcon && !root._hasText ? implicitHeight : contentRow.implicitWidth + 2 * root.horizontalPadding
    implicitHeight: 36
    radius: 6

    readonly property bool _raised: root.hovered || root.showFocus

    color: {
        switch (root.variant) {
        case "primary":
            return root._raised ? Theme.buttonPrimaryHoverBg : Theme.buttonPrimaryBg;
        case "secondary":
            return Config.withAlpha(Theme.foreground, root._raised ? 0.12 : 0.07);
        case "accent":
            return root._raised ? Qt.lighter(Theme.accent, 1.08) : Theme.accent;
        case "tinted":
            return Config.withAlpha(root.foreground, root._raised ? 0.24 : 0.14);
        case "ghost":
        default:
            return root._raised ? Config.withAlpha(Theme.foreground, 0.08) : "transparent";
        }
    }

    border.width: root.bordered || root.showFocus ? 1 : 0
    border.color: {
        if (root.showFocus) {
            if (root.variant === "tinted")
                return Config.withAlpha(root.foreground, 0.6);
            return Config.withAlpha(root.variant === "accent" ? Theme.listItemSelectionFg : Theme.buttonFocusOutline, Config.windowOpacity);
        }
        if (!root.bordered)
            return "transparent";
        if (root.variant === "secondary")
            return Config.withAlpha(Theme.inputBorder, Config.windowOpacity);
        return Config.withAlpha(Theme.divider, Config.windowOpacity);
    }

    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: root._hasText && root._hasIcon ? 8 : 0

        ViciImage {
            visible: root._hasIcon
            source: root.iconSource !== undefined ? root.iconSource : (root.icon !== "" ? Img.builtin(root.icon) : "")
            width: root.iconSize
            height: root.iconSize
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            visible: root._hasText
            text: root.text
            color: root.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
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

    Keys.onReturnPressed: root.clicked()
    Keys.onSpacePressed: root.clicked()
}
