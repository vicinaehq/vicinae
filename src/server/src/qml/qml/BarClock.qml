import QtQuick

BarPill {
    id: root

    interactive: false
    tooltip: bar.longDate

    Row {
        spacing: 8

        Text {
            text: bar.date
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize + 1
            font.weight: Font.Normal
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: bar.time
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize + 1
            font.weight: Font.Normal
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
