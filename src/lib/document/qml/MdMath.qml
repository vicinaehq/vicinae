pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae.Documents

Flickable {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style
    required property var blockData
    property real fontSize: root.style.regularFontSize

    width: parent?.width ?? 0
    implicitHeight: equation.contentHeight + 12
    contentWidth: Math.max(width, equation.width)
    contentHeight: height
    clip: true
    acceptedButtons: Qt.NoButton
    interactive: contentWidth > width
    flickableDirection: Flickable.HorizontalFlick
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.horizontal: DocumentScrollBar {
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
