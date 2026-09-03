import QtQuick

/// Hover-highlighted container every bar widget sits in.
Item {
    id: root

    property int horizontalPadding: 7
    property bool highlighted: false
    property bool interactive: true
    property string tooltip: ""
    readonly property bool hovered: mouseArea.containsMouse
    default property alias content: contentItem.data

    signal clicked(var mouse)
    signal wheel(var wheel)

    implicitWidth: contentItem.childrenRect.width + 2 * horizontalPadding
    implicitHeight: 28
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        radius: 6
        visible: root.highlighted || (root.interactive && root.hovered)
        color: root.highlighted ? Config.withAlpha(Theme.accent, root.hovered ? 0.3 : 0.22) : Config.withAlpha(Theme.listItemHoverBg, Config.windowOpacity)
    }

    Item {
        id: contentItem
        x: root.horizontalPadding
        anchors.verticalCenter: parent.verticalCenter
        width: childrenRect.width
        height: childrenRect.height
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: root.interactive ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        onClicked: mouse => root.clicked(mouse)
        onWheel: wheel => root.wheel(wheel)
    }

    ViciToolTip {
        visible: root.tooltip !== "" && root.hovered
        text: root.tooltip
        PopupPlacement.alignment: Qt.AlignHCenter | Qt.AlignBottom
    }
}
