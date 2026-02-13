import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: 41

    required property string itemTitle
    required property string itemSubtitle
    required property string itemIconSource
    required property string itemAlias
    required property bool itemIsActive
    required property bool selected
    property string itemAccessory: ""

    signal clicked()
    signal doubleClicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        onDoubleClicked: root.doubleClicked()
    }

    // Selection / hover highlight with padding and rounding
    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 6
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg
                return Qt.rgba(c.r, c.g, c.b, 0.7)
            }
            if (mouseArea.containsMouse) {
                var h = Theme.listItemHoverBg
                return Qt.rgba(h.r, h.g, h.b, 0.5)
            }
            return "transparent"
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        Item {
            Layout.preferredWidth: 25
            Layout.preferredHeight: 25
            Layout.alignment: Qt.AlignVCenter

            Image {
                anchors.fill: parent
                source: root.itemIconSource
                sourceSize.width: 25
                sourceSize.height: 25
                asynchronous: true
                cache: true
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

        Text {
            Layout.fillWidth: true
            text: root.itemTitle
            color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
            font.pointSize: Theme.regularFontSize
            elide: Text.ElideRight
            maximumLineCount: 1
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
            visible: root.itemAccessory !== ""
            text: root.itemAccessory
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
    }
}
