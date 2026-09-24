pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SettingsRow {
    id: root
    required property string subpage

    ViciButton {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        implicitHeight: 26
        horizontalPadding: 10
        variant: "ghost"
        bordered: true
        text: qsTr("Manage")
        onClicked: Settings.currentSubpage = root.subpage
    }
}
