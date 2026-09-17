pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SearchBar {
    id: root
    horizontalPadding: 20
    flatAccessories: true

    MouseArea {
        anchors.fill: parent
        z: -1
        onClicked: root.focusInput()
        onWheel: wheel => wheel.accepted = false
    }
}
