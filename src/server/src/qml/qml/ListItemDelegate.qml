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

        Item {
            id: textRow
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            implicitHeight: titleText.implicitHeight

            readonly property real spacing: 6
            readonly property real aliasSpace: aliasBadge.visible ? aliasBadge.width + spacing : 0
            readonly property real availableForText: width - aliasSpace
            readonly property real subtitleReserved: subtitleText.visible ? Math.min(subtitleText.implicitWidth + spacing, availableForText * 0.5) : 0

            Text {
                id: titleText
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: Math.min(implicitWidth, textRow.availableForText - textRow.subtitleReserved)
                text: root.itemTitle
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Text {
                id: subtitleText
                visible: root.itemSubtitle !== ""
                anchors.left: titleText.right
                anchors.leftMargin: visible ? textRow.spacing : 0
                anchors.verticalCenter: parent.verticalCenter
                width: Math.min(implicitWidth, Math.max(0, textRow.availableForText - titleText.width - textRow.spacing))
                text: root.itemSubtitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            TextBadge {
                id: aliasBadge
                visible: root.itemAlias !== ""
                anchors.left: subtitleText.visible ? subtitleText.right : titleText.right
                anchors.leftMargin: visible ? textRow.spacing : 0
                anchors.verticalCenter: parent.verticalCenter
                text: root.itemAlias
            }
        }

        ListAccessoryRow {
            accessories: {
                if (root.itemAccessory instanceof Array)
                    return root.itemAccessory;
                if (typeof root.itemAccessory === "string" && root.itemAccessory !== "")
                    return [
                        {
                            text: root.itemAccessory,
                            color: root.itemAccessoryColor,
                            fill: false
                        }
                    ];
                return [];
            }
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            Layout.maximumWidth: implicitWidth
            Layout.alignment: Qt.AlignVCenter
            clip: true
        }
    }
}
