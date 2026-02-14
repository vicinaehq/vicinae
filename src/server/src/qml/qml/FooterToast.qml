import QtQuick

Row {
    spacing: 6

    // Toast indicator: color circle or spinning ring
    Item {
        width: 12
        height: 12
        anchors.verticalCenter: parent.verticalCenter

        // Static color circle (Success/Info/Warning/Danger)
        Rectangle {
            visible: launcher.toastStyle !== 4
            width: 10
            height: 10
            radius: 5
            anchors.centerIn: parent
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

        // Spinning ring (Dynamic, style=4)
        Rectangle {
            id: spinner
            visible: launcher.toastStyle === 4
            width: 12
            height: 12
            radius: 6
            color: "transparent"
            border.width: 2
            border.color: Theme.textMuted
            anchors.centerIn: parent

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
    }

    Text {
        text: launcher.toastTitle
        color: Theme.foreground
        font.pointSize: Theme.smallerFontSize
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        text: launcher.toastMessage
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
        anchors.verticalCenter: parent.verticalCenter
        visible: launcher.toastMessage !== ""
    }
}
