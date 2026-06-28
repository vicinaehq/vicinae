import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: 32

    property bool selected: false
    readonly property bool hovered: mouseArea.containsMouse && HoverActivation.active

    required property string title
    required property string iconSource
    required property var shortcutTokens
    required property bool isSubmenu
    required property bool isDanger

    signal clicked

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    SourceBlendRect {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        backgroundColor: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, Config.windowOpacity)
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg;
                return Qt.rgba(c.r, c.g, c.b, Config.windowOpacity);
            }
            if (root.hovered) {
                var h = Theme.listItemHoverBg;
                return Qt.rgba(h.r, h.g, h.b, Config.windowOpacity);
            }
            var bg = Theme.popoverBackground;
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        Item {
            visible: root.iconSource !== ""
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            Layout.alignment: Qt.AlignVCenter

            ViciImage {
                anchors.fill: parent
                source: root.iconSource
            }
        }

        Text {
            text: root.title
            color: {
                if (root.isDanger)
                    return Theme.danger;
                if (root.selected)
                    return Theme.listItemSelectionFg;
                return Theme.foreground;
            }
            font.pointSize: Theme.regularFontSize
            elide: Text.ElideRight
            maximumLineCount: 1
            Layout.fillWidth: true
        }

        ShortcutBadge {
            visible: root.shortcutTokens && root.shortcutTokens.length > 0
            tokens: root.shortcutTokens
            contentColor: root.selected ? Theme.listItemSelectionFg : Theme.foreground
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
