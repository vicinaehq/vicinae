pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Flickable {
    id: root
    required property var blockData
    property real fontSize: Theme.regularFontSize

    width: parent?.width ?? 0
    implicitHeight: equation.contentHeight + 12
    contentWidth: Math.max(width, equation.width)
    contentHeight: height
    clip: true
    acceptedButtons: Qt.NoButton
    interactive: contentWidth > width
    flickableDirection: Flickable.HorizontalFlick
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.horizontal: ViciScrollBar {
        policy: root.contentWidth > root.width ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    DocumentText {
        id: equation
        y: 6
        width: contentWidth
        x: Math.max(0, (root.width - width) / 2)
        fontSize: root.fontSize
        textFormat: TextEdit.RichText
        wrapMode: TextEdit.NoWrap
        text: root.blockData.html ?? ""
    }
}
