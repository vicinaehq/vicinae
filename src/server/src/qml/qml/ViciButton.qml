import QtQuick

Rectangle {
    id: root

    property string text: ""
    property string icon: ""
    property var iconSource
    property string variant: "ghost"
    property bool bordered: root.variant === "secondary"
    property color foreground: root.variant === "accent" ? Theme.listItemSelectionFg : Theme.foreground
    property real iconSize: 16
    property real horizontalPadding: 12
    property bool translucent: false

    signal clicked

    readonly property bool hovered: mouseArea.containsMouse
    readonly property bool _hasIcon: root.icon !== "" || root.iconSource !== undefined
    readonly property bool _hasText: root.text !== ""

    implicitWidth: root._hasIcon && !root._hasText ? implicitHeight : contentRow.implicitWidth + 2 * root.horizontalPadding
    implicitHeight: 36
    radius: 6

    function _applyTranslucency(c) {
        return root.translucent ? Qt.rgba(c.r, c.g, c.b, 0.6) : c;
    }

    color: {
        switch (root.variant) {
        case "primary":
            return root._applyTranslucency(root.hovered ? Theme.buttonPrimaryHoverBg : Theme.buttonPrimaryBg);
        case "secondary":
            return root._applyTranslucency(root.hovered ? Theme.listItemHoverBg : Theme.secondaryBackground);
        case "accent":
            return root._applyTranslucency(root.hovered ? Qt.lighter(Theme.accent, 1.15) : Theme.accent);
        case "ghost":
        default:
            return root.hovered ? Theme.listItemHoverBg : "transparent";
        }
    }

    border.width: root.bordered || root.activeFocus ? 1 : 0
    border.color: {
        if (root.activeFocus)
            return root.variant === "accent" ? Theme.listItemSelectionFg : Theme.accent;
        if (!root.bordered)
            return "transparent";
        if (root.variant === "secondary")
            return Theme.inputBorder;
        return Theme.divider;
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
