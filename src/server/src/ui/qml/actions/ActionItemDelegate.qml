pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    height: 32

    property bool selected: false
    property PopupMaterialStyle popupStyle: null
    readonly property bool hovered: mouseArea.containsMouse && HoverActivation.active

    required property string title
    required property string iconSource
    required property var shortcutTokens
    required property bool isSubmenu
    required property bool isDanger

    signal clicked

    Accessible.role: Accessible.MenuItem
    Accessible.name: root.title
    Accessible.selectable: true
    Accessible.selected: root.selected
    Accessible.onPressAction: root.clicked()

    readonly property var _win: root.Window.window
    // Qt.Tool contains the Qt.Popup bit, so mask the full window type or the
    // launcher window itself matches for in-scene popups
    readonly property bool _nativeWindow: _win !== null && (_win.flags & Qt.WindowType_Mask) === Qt.Popup

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }

    PopupItemBackground {
        id: itemBackground
        style: root.popupStyle
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        nativeWindow: root._nativeWindow
        selected: root.selected
        hovered: root.hovered
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
                    return itemBackground.selectedText;
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
            contentColor: root.selected ? itemBackground.selectedText : Theme.foreground
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
