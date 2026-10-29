pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

MenuItem {
    id: root

    implicitHeight: 30
    leftPadding: 10
    rightPadding: root.checkable ? 10 + 16 + 8 : 10

    indicator: ViciImage {
        visible: root.checkable && root.checked
        x: root.width - root.rightPadding + 8
        anchors.verticalCenter: parent.verticalCenter
        width: 16
        height: 16
        source: Img.icon(BuiltinIcon.Checkmark).withFillColor(root.highlighted ? Theme.listItemSelectionFg : Theme.foreground)
    }

    contentItem: Text {
        text: root.text
        color: !root.enabled ? Theme.textMuted : root.highlighted ? Theme.listItemSelectionFg : Theme.foreground
        font.pointSize: Theme.regularFontSize
        elide: Text.ElideRight
        maximumLineCount: 1
        verticalAlignment: Text.AlignVCenter
    }

    background: PopupItemBackground {
        radius: 6
        selected: root.highlighted && root.enabled
    }
}
