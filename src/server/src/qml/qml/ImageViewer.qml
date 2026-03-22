import QtQuick
import QtQuick.Controls

Popup {
    id: root
    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Overlay.overlay ? Overlay.overlay.width : 0
    height: Overlay.overlay ? Overlay.overlay.height : 0
    padding: 0
    focus: true
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property var sources: []
    property int currentIndex: 0

    component CircleButton: Rectangle {
        property alias icon: _btnIcon.source
        property alias hovered: _btnMouseArea.containsMouse

        width: 24
        height: 24
        radius: 12
        color: _btnMouseArea.containsMouse ? Qt.rgba(1, 1, 1, 0.25) : Qt.rgba(1, 1, 1, 0.1)

        signal clicked()

        ViciImage {
            id: _btnIcon
            anchors.centerIn: parent
            width: 12
            height: 12
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
        open();
    }

    onOpened: _focusItem.forceActiveFocus()

    enter: Transition {
        NumberAnimation {
            property: "opacity"
            from: 0
            to: 1
            duration: 150
            easing.type: Easing.OutCubic
        }
    }

    exit: Transition {
        NumberAnimation {
            property: "opacity"
            from: 1
            to: 0
            duration: 100
            easing.type: Easing.InCubic
        }
    }

    background: Item {}

    Overlay.modal: Rectangle {
        radius: Config.borderRounding
        color: Qt.rgba(0, 0, 0, 0.92)
    }

    contentItem: Item {
        id: _focusItem
        focus: true

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Left && root.currentIndex > 0) {
                root.currentIndex--;
                event.accepted = true;
            } else if (event.key === Qt.Key_Right && root.currentIndex < root.sources.length - 1) {
                root.currentIndex++;
                event.accepted = true;
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: root.close()
        }

        ViciImage {
            id: _image
            anchors.centerIn: parent
            width: parent.width - 48
            height: parent.height - 48
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
            anchors.leftMargin: 8
            icon: Img.builtin("chevron-left-small").withFillColor("white")
            onClicked: root.currentIndex--
        }

        CircleButton {
            visible: root.sources.length > 1 && root.currentIndex < root.sources.length - 1
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 8
            icon: Img.builtin("chevron-right-small").withFillColor("white")
            onClicked: root.currentIndex++
        }

        CircleButton {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 8
            icon: Img.builtin("xmark").withFillColor("white")
            onClicked: root.close()
        }

        Rectangle {
            visible: root.sources.length > 1
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 10
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
}
