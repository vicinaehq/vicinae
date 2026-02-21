import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    property var accessories: []

    visible: accessories instanceof Array && accessories.length > 0
    spacing: 6

    Repeater {
        model: root.accessories

        ListAccessory {
            required property var modelData
            text: modelData["text"] || ""
            accentColor: modelData["color"] || ""
            fill: !!modelData["fill"]
            icon: modelData["icon"] || ""
            tooltip: modelData["tooltip"] || ""
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
