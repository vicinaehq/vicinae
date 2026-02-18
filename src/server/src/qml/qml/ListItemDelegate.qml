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
    property string itemAccessory: ""
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

            // Active indicator dot
            Rectangle {
                visible: root.itemIsActive
                width: 6
                height: 6
                radius: 3
                color: Theme.accent
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: -2
            }
        }

        // Title + subtitle + alias grouped together on the left
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

            Text {
                visible: root.itemAlias !== ""
                text: root.itemAlias
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                font.italic: true
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Item { Layout.fillWidth: true }
        }

        Text {
            visible: root.itemAccessory !== ""
            text: root.itemAccessory
            color: root.itemAccessoryColor !== "" ? root.itemAccessoryColor : Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            elide: Text.ElideRight
            maximumLineCount: 1
            Layout.maximumWidth: 200
        }
    }
}
