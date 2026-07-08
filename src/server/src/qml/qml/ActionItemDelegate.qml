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

    readonly property var _win: root.Window.window
    readonly property bool _nativeWindow: _win !== null && (_win.flags & Qt.Popup) === Qt.Popup
    readonly property real _opacity: root._nativeWindow ? Config.popupOpacity : 1
    readonly property real _fillOpacity: root._nativeWindow ? Config.popupSurfaceOpacity : 1

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
        backgroundColor: root._nativeWindow ? Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, Config.popupOpacity) : "transparent"
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg;
                return Qt.rgba(c.r, c.g, c.b, root._fillOpacity);
            }
            if (root.hovered) {
                var h = Theme.listItemHoverBg;
                return Qt.rgba(h.r, h.g, h.b, root._fillOpacity);
            }
            var bg = Theme.popoverBackground;
            return Qt.rgba(bg.r, bg.g, bg.b, root._opacity);
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
