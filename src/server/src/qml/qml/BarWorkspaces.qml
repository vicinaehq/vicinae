import QtQuick
import QtQuick.Layouts

Row {
    id: root

    required property var model
    spacing: 2

    Repeater {
        model: root.model

        BarPill {
            id: pill
            required property int index
            required property string name
            required property bool active
            required property bool hasWindows
            required property int windowCount
            required property var icons

            highlighted: pill.active
            horizontalPadding: 8
            tooltip: pill.windowCount > 0 ? qsTr("%n window(s)", "", pill.windowCount) : qsTr("Empty")
            onClicked: root.model.focus(pill.index)

            Row {
                spacing: 5

                Text {
                    text: pill.name
                    color: pill.active ? Theme.foreground : Theme.textMuted
                    opacity: pill.active || pill.hasWindows ? 1 : 0.7
                    font.family: Theme.fontFamily
                    font.pointSize: Theme.regularFontSize + 1
                    font.weight: pill.active ? Font.Medium : Font.Normal
                    anchors.verticalCenter: parent.verticalCenter
                }

                Row {
                    spacing: 3
                    visible: pill.icons.length > 0
                    anchors.verticalCenter: parent.verticalCenter

                    Repeater {
                        model: pill.icons

                        ViciImage {
                            required property var modelData
                            width: 18
                            height: 18
                            source: modelData
                            sourceSize: Qt.size(18, 18)
                            opacity: pill.active ? 1 : 0.6
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
