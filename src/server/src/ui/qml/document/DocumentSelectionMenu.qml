pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

ViciMenu {
    id: root
    required property DocumentController controller

    ViciMenuItem {
        text: qsTr("Copy")
        enabled: root.controller.hasSelection
        onTriggered: root.controller.copy()
    }
    ViciMenuItem {
        text: qsTr("Select All")
        onTriggered: root.controller.selectAll()
    }
}
