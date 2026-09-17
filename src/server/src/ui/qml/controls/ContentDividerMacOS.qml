pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root
    property bool vertical: false
    readonly property color lineColor: Config.withAlpha(Theme.divider, Theme.isDark ? 0.28 : 0.16)
    readonly property real inset: Math.min(12, (vertical ? height : width) * 0.1)
    readonly property int fadeLength: 20
    implicitWidth: vertical ? 1 : -1
    implicitHeight: vertical ? -1 : 1

    Rectangle {
        id: line
        anchors.fill: parent
        anchors.leftMargin: root.vertical ? 0 : root.inset
        anchors.rightMargin: anchors.leftMargin
        anchors.topMargin: root.vertical ? root.inset : 0
        anchors.bottomMargin: anchors.topMargin

        readonly property real fade: Math.min(0.25, root.fadeLength / Math.max(1, root.vertical ? height : width))

        gradient: Gradient {
            orientation: root.vertical ? Gradient.Vertical : Gradient.Horizontal
            GradientStop {
                position: 0
                color: line.fade > 0 ? "transparent" : root.lineColor
            }
            GradientStop {
                position: line.fade
                color: root.lineColor
            }
            GradientStop {
                position: 1 - line.fade
                color: root.lineColor
            }
            GradientStop {
                position: 1
                color: line.fade > 0 ? "transparent" : root.lineColor
            }
        }
    }
}
