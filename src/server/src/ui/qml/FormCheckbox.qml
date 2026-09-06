import QtQuick
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: Math.max(28, contentRow.implicitHeight)
    Layout.fillWidth: true
    activeFocusOnTab: !readOnly

    property bool checked: false
    property string label: ""
    property bool readOnly: false
    property bool hasError: false
    property bool filled: false

    signal toggled

    Accessible.role: Accessible.CheckBox
    Accessible.name: root.label
    Accessible.checkable: true
    Accessible.checked: root.checked
    Accessible.onToggleAction: root.toggle()

    function toggle() {
        if (root.readOnly)
            return;
        root.checked = !root.checked;
        root.toggled();
    }

    Keys.onSpacePressed: toggle()
    Keys.onReturnPressed: toggle()

    MouseArea {
        anchors.fill: parent
        cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
        onClicked: root.toggle()
    }

    FormInputBackground {
        width: 16
        height: 16
        radius: 4
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        filled: root.filled && !root.checked
        opacity: root.readOnly ? 0.5 : 1.0
    }

    RowLayout {
        id: contentRow
        anchors.fill: parent
        spacing: 8
        opacity: root.readOnly ? 0.5 : 1.0

        Rectangle {
            id: box
            width: 16
            height: 16
            radius: 4
            antialiasing: true
            Layout.alignment: Qt.AlignVCenter
            color: "transparent"
            gradient: root.checked ? boxFill : null
            border.color: Config.withAlpha(root.hasError ? Theme.inputBorderError : root.activeFocus ? Theme.inputBorderFocus : root.checked ? Theme.accent : Theme.inputBorder, Config.surfaceOpacity)
            border.width: 1

            function _shifted(c, dh, ds, dl) {
                const h = (c.hslHue < 0 ? 0 : c.hslHue) + dh;
                const clamp = v => Math.min(1, Math.max(0, v));
                return Qt.hsla((h + 1) % 1, clamp(c.hslSaturation + ds), clamp(c.hslLightness + dl), c.a);
            }

            Gradient {
                id: boxFill
                GradientStop {
                    position: 0
                    color: box._shifted(Theme.accent, 0.025, -0.03, 0.06)
                }
                GradientStop {
                    position: 1
                    color: box._shifted(Theme.accent, -0.015, 0.04, -0.04)
                }
            }

            Text {
                anchors.centerIn: parent
                text: "\u2713"
                color: "#ffffff"
                style: Text.Raised
                styleColor: Qt.rgba(0, 0, 0, 0.3)
                font.pixelSize: 11
                font.bold: true
                visible: root.checked
            }
        }

        Text {
            visible: root.label !== ""
            text: root.label
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }
    }
}
