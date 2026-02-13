import QtQuick
import QtQuick.Layouts

Item {
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
