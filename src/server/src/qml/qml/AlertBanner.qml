import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    property string alertType: "info"
    property string message: ""

    readonly property color _alertColor: {
        switch (alertType) {
        case "danger": return Theme.toastDanger
        case "warning": return Theme.toastWarning
        default: return Theme.toastInfo
        }
    }

    readonly property var _iconSource: {
        switch (alertType) {
        case "danger": return Img.builtin("warning").withFillColor(root._alertColor)
        case "warning": return Img.builtin("warning").withFillColor(root._alertColor)
        default: return Img.builtin("info-01").withFillColor(root._alertColor)
        }
    }

    color: Qt.rgba(_alertColor.r, _alertColor.g, _alertColor.b, 0.15)
    radius: 6
    implicitHeight: _layout.implicitHeight + 16

    RowLayout {
        id: _layout
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        ViciImage {
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            Layout.alignment: Qt.AlignTop
            source: root._iconSource
        }

        Text {
            Layout.fillWidth: true
            text: root.message
            color: Theme.foreground
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.WordWrap
        }
    }
}
