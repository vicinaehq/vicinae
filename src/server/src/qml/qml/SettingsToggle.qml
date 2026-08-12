import QtQuick

Item {
    id: root
    implicitWidth: 36
    implicitHeight: 20
    anchors.right: parent ? parent.right : undefined
    activeFocusOnTab: true

    property bool checked: false
    signal toggled(bool checked)

    function _shifted(c, dh, ds, dl) {
        const h = (c.hslHue < 0 ? 0 : c.hslHue) + dh;
        const clamp = v => Math.min(1, Math.max(0, v));
        return Qt.hsla((h + 1) % 1, clamp(c.hslSaturation + ds), clamp(c.hslLightness + dl), c.a);
    }

    Rectangle {
        id: track
        anchors.fill: parent
        radius: 10
        antialiasing: true

        property color base: root.checked ? Theme.accent : Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.2)
        Behavior on base {
            ColorAnimation {
                duration: 120
            }
        }

        gradient: Gradient {
            GradientStop {
                position: 0
                color: root._shifted(track.base, 0.025, -0.03, 0.06)
            }
            GradientStop {
                position: 1
                color: root._shifted(track.base, -0.015, 0.04, -0.04)
            }
        }

        border.width: root.activeFocus ? 1 : 0
        border.color: Config.withAlpha(Theme.inputBorderFocus, Config.surfaceOpacity)

        Rectangle {
            width: 16
            height: 16
            radius: 8
            antialiasing: true
            x: knob.x
            y: knob.y + 1
            color: Qt.rgba(0, 0, 0, 0.25)
        }

        Rectangle {
            id: knob
            width: 16
            height: 16
            radius: 8
            antialiasing: true
            x: root.checked ? parent.width - width - 2 : 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#ffffff"
            Behavior on x {
                NumberAnimation {
                    duration: 120
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.toggled(!root.checked)
    }

    Keys.onReturnPressed: root.toggled(!root.checked)
    Keys.onSpacePressed: root.toggled(!root.checked)
}
