import QtQuick
import QtQuick.Layouts

RowLayout {
    spacing: 6

    Rectangle {
        visible: launcher.toastStyle !== 4
        Layout.preferredWidth: 10
        Layout.preferredHeight: 10
        Layout.alignment: Qt.AlignVCenter
        radius: 5
        color: {
            switch (launcher.toastStyle) {
                case 0: return Theme.toastSuccess
                case 1: return Theme.toastInfo
                case 2: return Theme.toastWarning
                case 3: return Theme.toastDanger
                default: return Theme.toastInfo
            }
        }
    }

    Rectangle {
        id: spinner
        visible: launcher.toastStyle === 4
        Layout.preferredWidth: 12
        Layout.preferredHeight: 12
        Layout.alignment: Qt.AlignVCenter
        radius: 6
        color: "transparent"
        border.width: 2
        border.color: Theme.textMuted

        Rectangle {
            width: 6
            height: 6
            color: Theme.statusBarBackground
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: -1
            anchors.rightMargin: -1
        }

        RotationAnimation on rotation {
            running: spinner.visible
            from: 0
            to: 360
            duration: 1000
            loops: Animation.Infinite
        }
    }

    Text {
        text: launcher.toastTitle
        color: Theme.foreground
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
    }

    Text {
        text: launcher.toastMessage
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
        visible: launcher.toastMessage !== ""
        maximumLineCount: 1
        elide: Text.ElideRight
        Layout.fillWidth: true
    }

    Item { Layout.fillWidth: true }
}
