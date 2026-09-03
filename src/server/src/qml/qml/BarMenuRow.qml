import QtQuick
import QtQuick.Layouts

Item {
    id: root

    required property string label
    property var iconSource
    property bool showIconSlot: false
    property bool submenu: false
    property int toggleType: 0
    property bool checked: false

    signal clicked

    implicitWidth: row.implicitWidth + 20
    implicitHeight: 28
    opacity: enabled ? 1 : 0.4

    Rectangle {
        anchors.fill: parent
        radius: 6
        visible: mouseArea.containsMouse && root.enabled
        color: Theme.listItemHoverBg
    }

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 8

        Item {
            visible: root.toggleType !== 0
            Layout.preferredWidth: 12
            Layout.preferredHeight: 12

            ViciImage {
                anchors.fill: parent
                visible: root.checked
                source: Img.builtin(root.toggleType === 2 ? "circle" : "check").withFillColor(Theme.foreground)
                sourceSize: Qt.size(12, 12)
            }
        }

        ViciImage {
            visible: root.showIconSlot
            source: root.iconSource ?? ""
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            sourceSize: Qt.size(16, 16)
        }

        Text {
            text: root.label
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.smallerFontSize
            elide: Text.ElideRight
            maximumLineCount: 1
            Layout.fillWidth: true
        }

        ViciImage {
            visible: root.submenu
            source: Img.builtin("chevron-right").withFillColor(Theme.textMuted)
            Layout.preferredWidth: 12
            Layout.preferredHeight: 12
            sourceSize: Qt.size(12, 12)
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: if (root.enabled)
            root.clicked()
    }
}
