import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property string title: "No results"
    property string description: ""
    property string icon: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.foreground

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(400, parent.width)
        spacing: 10

        ViciImage {
            visible: root.icon !== ""
            source: root.icon
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            visible: root.title !== ""
            text: root.title
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        Text {
            visible: root.description !== ""
            text: root.description
            color: Theme.textMuted
            font.pointSize: Theme.regularFontSize
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
    }
}
