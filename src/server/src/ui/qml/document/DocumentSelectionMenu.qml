pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import Vicinae

Menu {
    id: root
    required property DocumentController controller
    popupType: Popup.Native

    MenuItem {
        text: qsTr("Copy")
        enabled: root.controller.hasSelection
        onTriggered: root.controller.copy()
    }
    MenuItem {
        text: qsTr("Select All")
        onTriggered: root.controller.selectAll()
    }
}
