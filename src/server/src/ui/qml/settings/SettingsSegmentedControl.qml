pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

// A row of mutually exclusive options. `options` is a list of { id, label }.
Rectangle {
    id: root

    property var options: []
    property string currentId: ""
    property string accessibleLabel: ""
    signal activated(string id)

    implicitWidth: row.implicitWidth + 6
    implicitHeight: 30
    radius: 8
    color: Config.withAlpha(Theme.foreground, 0.05)
    border.color: Config.withAlpha(Theme.divider, Config.surfaceOpacity)
    border.width: 1

    Accessible.role: Accessible.PageTabList
    Accessible.name: root.accessibleLabel

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: 3
        spacing: 2

        Repeater {
            model: root.options

            delegate: Rectangle {
                id: segment
                required property var modelData
                readonly property bool selected: modelData.id === root.currentId

                Layout.fillHeight: true
                implicitWidth: label.implicitWidth + 28
                radius: 6
                color: segment.selected ? Config.withAlpha(Theme.foreground, 0.12) : mouse.containsMouse ? Config.withAlpha(Theme.foreground, 0.06) : "transparent"

                Accessible.role: Accessible.PageTab
                Accessible.name: segment.modelData.label
                Accessible.checked: segment.selected

                Text {
                    id: label
                    anchors.centerIn: parent
                    text: segment.modelData.label
                    color: segment.selected ? Theme.foreground : Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                }

                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (!segment.selected)
                            root.activated(segment.modelData.id);
                    }
                }
            }
        }
    }
}
