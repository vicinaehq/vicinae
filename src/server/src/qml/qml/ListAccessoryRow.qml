import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root
    property var accessories
    property string fallbackColor: ""

    readonly property bool _isString: typeof accessories === "string"
    readonly property var _list: accessories instanceof Array ? accessories : []
    visible: (_isString && accessories !== "") || _list.length > 0
    spacing: 6

    Text {
        visible: root._isString && root.accessories !== ""
        text: root._isString ? root.accessories : ""
        color: root.fallbackColor !== "" ? root.fallbackColor : Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        elide: Text.ElideRight
        maximumLineCount: 1
        Layout.maximumWidth: 200
    }

    Repeater {
        model: root._list

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
