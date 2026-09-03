import QtQuick
import QtQuick.Layouts
import org.kde.layershell as LayerShell

Window {
    id: root

    readonly property int barHeight: 34
    readonly property int edgePadding: 10
    readonly property int sectionSpacing: 6

    width: Screen.width
    height: barHeight
    flags: Qt.FramelessWindowHint
    color: "transparent"
    visible: false
    title: qsTr("Vicinae Bar")

    LayerShell.Window.layer: LayerShell.Window.LayerTop
    LayerShell.Window.scope: "vicinae-bar"
    LayerShell.Window.anchors: LayerShell.Window.AnchorTop | LayerShell.Window.AnchorLeft | LayerShell.Window.AnchorRight
    LayerShell.Window.exclusionZone: root.barHeight
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone

    Rectangle {
        anchors.fill: parent
        color: Theme.background
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Theme.mainWindowBorder
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: root.edgePadding
        anchors.rightMargin: root.edgePadding

        RowLayout {
            id: leftSection
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: root.sectionSpacing

            BarLauncherButton {}

            BarWorkspaces {
                model: barScreen.workspaces
            }
        }

        BarFocusedWindow {
            anchors.centerIn: parent
            maxWidth: Math.max(0, Math.min(360, parent.width / 4, parent.width - 2 * (Math.max(leftSection.width, rightSection.width) + 2 * root.sectionSpacing)))
        }

        RowLayout {
            id: rightSection
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            spacing: 10

            BarMedia {}

            BarTray {}

            BarVolume {}

            BarPower {}

            BarClock {}
        }
    }
}
