import QtQuick
import QtQuick.Controls
import QtQuick.Window

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: Qt.rgba(0, 0, 0, 0.92)
    visible: false
    x: Screen.virtualX
    y: Screen.virtualY
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight

    property var sources: []
    property int currentIndex: 0

    component CircleButton: Rectangle {
        property alias icon: _btnIcon.source
        property alias hovered: _btnMouseArea.containsMouse

        width: 36
        height: 36
        radius: 18
        color: _btnMouseArea.containsMouse ? Qt.rgba(1, 1, 1, 0.25) : Qt.rgba(1, 1, 1, 0.1)

        signal clicked()

        ViciImage {
            id: _btnIcon
            anchors.centerIn: parent
            width: 16
            height: 16
        }

        MouseArea {
            id: _btnMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    function showImage(index, imageSources) {
        sources = imageSources;
        currentIndex = index;
        visible = true;
        requestActivate();
        _focusItem.forceActiveFocus();
    }

    onActiveChanged: {
        if (active)
            _focusItem.forceActiveFocus();
    }

    Item {
        id: _focusItem
        anchors.fill: parent
        focus: true

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Escape) {
                root.visible = false;
                event.accepted = true;
            } else if (event.key === Qt.Key_Left && root.currentIndex > 0) {
                root.currentIndex--;
                event.accepted = true;
            } else if (event.key === Qt.Key_Right && root.currentIndex < root.sources.length - 1) {
                root.currentIndex++;
                event.accepted = true;
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.visible = false
    }

    ViciImage {
        id: _image
        anchors.centerIn: parent
        width: root.width - 80
        height: root.height - 80
        source: root.sources[root.currentIndex] ?? ""
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors.fill: parent
            onClicked: event => event.accepted = true
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: _image.status === Image.Loading
        visible: running
    }

    CircleButton {
        visible: root.sources.length > 1 && root.currentIndex > 0
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
        icon: Img.builtin("chevron-left-small").withFillColor("white")
        onClicked: root.currentIndex--
    }

    CircleButton {
        visible: root.sources.length > 1 && root.currentIndex < root.sources.length - 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 16
        icon: Img.builtin("chevron-right-small").withFillColor("white")
        onClicked: root.currentIndex++
    }

    CircleButton {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.rightMargin: 16
        icon: Img.builtin("xmark").withFillColor("white")
        onClicked: root.visible = false
    }

    Rectangle {
        visible: root.sources.length > 1
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        implicitWidth: _counter.implicitWidth + 20
        implicitHeight: _counter.implicitHeight + 10
        radius: 12
        color: Qt.rgba(0, 0, 0, 0.6)

        Text {
            id: _counter
            anchors.centerIn: parent
            text: `${root.currentIndex + 1} / ${root.sources.length}`
            color: "white"
            font.pointSize: Theme.smallerFontSize
        }
    }
}
