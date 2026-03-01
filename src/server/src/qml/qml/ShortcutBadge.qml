import QtQuick

Row {
    id: root
    property string text: ""
    spacing: 4

    function iconForKey(key) {
        const map = {
            "Super": "command-symbol",
            "Ctrl": "chevron-up",
            "Alt": "option-symbol",
            "Shift": "keyboard-shift",
        };
        return map[key] ?? "";
    }

    Repeater {
        model: root.text !== "" ? root.text.split("+") : []

        Rectangle {
            required property string modelData
            readonly property string iconName: root.iconForKey(modelData)
            readonly property int iconSize: Math.round(keyLabel.implicitHeight * 0.85)

            width: Math.max(height, iconName
                ? iconSize + 14
                : keyLabel.implicitWidth + 14)
            height: keyLabel.implicitHeight + 8
            radius: 4
            color: "transparent"
            border.width: 1
            border.color: Theme.divider

            ViciImage {
                id: icon
                visible: parent.iconName !== ""
                source: parent.iconName !== ""
                    ? Img.builtin(parent.iconName).withFillColor(Theme.textMuted)
                    : ""
                width: parent.iconSize
                height: parent.iconSize
                anchors.centerIn: parent
            }

            Text {
                id: keyLabel
                visible: parent.iconName === ""
                text: parent.modelData
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pointSize: Theme.smallerFontSize - 1
                anchors.centerIn: parent
            }
        }
    }
}
