import QtQuick

Rectangle {
    id: root
    required property string text
    property string accentColor: ""
    property bool fill: false
    property string icon: ""
    property string tooltip: ""

    color: fill ? Qt.rgba(_resolvedColor.r, _resolvedColor.g, _resolvedColor.b, 0.2)
                : "transparent"
    radius: fill ? 4 : 0
    implicitWidth: contentRow.implicitWidth + (fill ? 12 : 0)
    implicitHeight: contentRow.implicitHeight + (fill ? 6 : 0)

    readonly property color _resolvedColor: accentColor !== "" ? accentColor : Theme.textMuted

    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: 4

        ViciImage {
            visible: root.icon !== ""
            source: root.icon
            width: 14
            height: 14
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: root.text
            color: root._resolvedColor
            font.pointSize: Theme.smallerFontSize
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: root.tooltip !== ""
        acceptedButtons: Qt.NoButton

        ViciToolTip {
            visible: parent.containsMouse && root.tooltip !== ""
            text: root.tooltip
        }
    }
}
