import QtQuick
import QtQuick.Shapes
import Vicinae

Shape {
    id: root

    property color color: Theme.textMuted
    property real strokeWidth: 2

    implicitWidth: 14
    implicitHeight: 14
    preferredRendererType: Shape.CurveRenderer

    ShapePath {
        strokeColor: root.color
        strokeWidth: root.strokeWidth
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap

        PathAngleArc {
            centerX: root.width / 2
            centerY: root.height / 2
            radiusX: root.width / 2 - root.strokeWidth / 2
            radiusY: root.height / 2 - root.strokeWidth / 2
            startAngle: 0
            sweepAngle: 270
        }
    }

    RotationAnimation on rotation {
        running: root.visible
        from: 0
        to: 360
        duration: 1000
        loops: Animation.Infinite
    }
}
