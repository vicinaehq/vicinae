import QtQuick
import QtQuick.Layouts

Item {
    id: root
    height: 60

    required property string itemTitle
    required property string itemSubtitle
    required property string itemIconSource
    required property bool selected

    property color paletteColor0: "transparent"
    property color paletteColor1: "transparent"
    property color paletteColor2: "transparent"
    property color paletteColor3: "transparent"
    property color paletteColor4: "transparent"
    property color paletteColor5: "transparent"
    property color paletteColor6: "transparent"
    property color paletteColor7: "transparent"

    signal clicked()
    signal doubleClicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        onDoubleClicked: root.doubleClicked()
    }

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

        // Icon
        Item {
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignVCenter

            Image {
                anchors.fill: parent
                source: root.itemIconSource
                sourceSize.width: 30
                sourceSize.height: 30
                asynchronous: true
                cache: true
            }
        }

        // Title + description stacked
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Text {
                text: root.itemTitle
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                Layout.fillWidth: true
            }

            Text {
                visible: root.itemSubtitle !== ""
                text: root.itemSubtitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                Layout.fillWidth: true
            }
        }

        // Color palette
        Row {
            spacing: 3
            Layout.alignment: Qt.AlignVCenter

            Repeater {
                model: [
                    root.paletteColor0, root.paletteColor1,
                    root.paletteColor2, root.paletteColor3,
                    root.paletteColor4, root.paletteColor5,
                    root.paletteColor6, root.paletteColor7
                ]
                delegate: Item {
                    width: 16
                    height: 16
                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: Theme.foreground
                        antialiasing: true
                    }
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 1
                        radius: width / 2
                        color: modelData
                        antialiasing: true
                    }
                }
            }
        }
    }
}
