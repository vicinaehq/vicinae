import QtQuick
import QtQuick.Layouts

Item {
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(Theme.statusBarBackground.r, Theme.statusBarBackground.g, Theme.statusBarBackground.b, 0.6)
        radius: 10

        // Mask out the top rounded corners by overlaying a rect on the top half
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height / 2
            color: parent.color
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 8

        Item { Layout.fillWidth: true }

        // Primary action label + Enter badge
        Row {
            visible: {
                if (launcher.hasCommandView) return launcher.commandActionTitle !== ""
                return searchModel.primaryActionTitle !== ""
            }
            spacing: 6
            Layout.alignment: Qt.AlignVCenter

            Text {
                text: launcher.hasCommandView ? launcher.commandActionTitle : searchModel.primaryActionTitle
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle {
                width: enterLabel.implicitWidth + 10
                height: enterLabel.implicitHeight + 4
                radius: 4
                color: Theme.divider
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: enterLabel
                    text: "Enter"
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize - 1
                    anchors.centerIn: parent
                }
            }
        }
    }
}
