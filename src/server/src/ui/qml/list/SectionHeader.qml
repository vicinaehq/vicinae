pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    readonly property LauncherAppearance appearance: (root.Window.window as LauncherWindow)?.appearance ?? fallbackAppearance
    height: appearance.sectionHeaderHeight

    LauncherAppearance {
        id: fallbackAppearance
    }

    required property string text
    property real leftPadding: 16

    Text {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.leftPadding
        anchors.rightMargin: root.leftPadding
        anchors.verticalCenter: parent.verticalCenter
        text: root.text
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        font.weight: Font.DemiBold
        elide: Text.ElideRight
    }
}
