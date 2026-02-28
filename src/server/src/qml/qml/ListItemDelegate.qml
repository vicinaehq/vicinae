import QtQuick
import QtQuick.Layouts

SelectableDelegate {
    id: root
    height: 41

    required property string itemTitle
    required property string itemSubtitle
    required property string itemIconSource
    required property string itemAlias
    required property bool itemIsActive
    property var itemAccessory: []
    property string itemAccessoryColor: ""

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        Item {
            visible: root.itemIconSource !== ""
            Layout.preferredWidth: 25
            Layout.preferredHeight: 25
            Layout.alignment: Qt.AlignVCenter

            ViciImage {
                anchors.fill: parent
                source: root.itemIconSource
            }

            Rectangle {
                visible: root.itemIsActive
                width: 4
                height: 4
                radius: 2
                color: Theme.textMuted
                anchors.top: parent.bottom
                anchors.topMargin: 3
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        RowLayout {
            id: textRow
            Layout.fillWidth: true
            spacing: 6

            Text {
                text: root.itemTitle
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                Layout.maximumWidth: Math.min(implicitWidth, root.width * 0.5)
            }

            Text {
                visible: root.itemSubtitle !== ""
                text: root.itemSubtitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                Layout.maximumWidth: 200
            }

            Rectangle {
                visible: root.itemAlias !== ""
                implicitWidth: aliasText.implicitWidth + 20
                implicitHeight: aliasText.implicitHeight + 4
                radius: 4
                color: "transparent"
                border.width: 1
                border.color: Theme.divider

                Text {
                    id: aliasText
                    anchors.centerIn: parent
                    text: root.itemAlias
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
            }

            Item { Layout.fillWidth: true }
        }

        ListAccessoryRow {
            accessories: {
                if (root.itemAccessory instanceof Array) return root.itemAccessory
                if (typeof root.itemAccessory === "string" && root.itemAccessory !== "")
                    return [{text: root.itemAccessory, color: root.itemAccessoryColor, fill: false}]
                return []
            }
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
