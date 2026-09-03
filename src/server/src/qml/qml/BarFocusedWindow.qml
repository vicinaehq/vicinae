import QtQuick

BarPill {
    id: root

    property real maxWidth: 560

    visible: bar.hasFocusedWindow
    interactive: false
    tooltip: bar.focusedTitle !== "" && bar.focusedTitle !== bar.focusedApp ? qsTr("%1 — %2").arg(bar.focusedApp).arg(bar.focusedTitle) : bar.focusedApp
    horizontalPadding: 6
    implicitWidth: Math.min(row.implicitWidth + 2 * horizontalPadding, maxWidth)
    width: implicitWidth

    Row {
        id: row
        spacing: 7

        ViciImage {
            width: 20
            height: 20
            source: bar.focusedIcon
            sourceSize: Qt.size(20, 20)
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: bar.focusedTitle !== "" ? bar.focusedTitle : bar.focusedApp
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize + 1
            font.weight: Font.Normal
            elide: Text.ElideRight
            maximumLineCount: 1
            width: Math.min(implicitWidth, Math.max(0, root.maxWidth - 2 * root.horizontalPadding - 27))
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
