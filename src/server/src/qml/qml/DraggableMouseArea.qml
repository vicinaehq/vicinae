import QtQuick

MouseArea {
    id: root

    property bool draggable: false
    readonly property bool dragging: dragStarted

    signal itemClicked
    signal itemActivated
    signal dragRequested

    property bool dragStarted: false

    drag.target: draggable ? dragTarget : null

    onPressed: {
        dragStarted = false;
        dragTarget.x = 0;
        dragTarget.y = 0;
    }
    onPositionChanged: {
        if (drag.active && !dragStarted) {
            dragStarted = true;
            dragRequested();
        }
    }
    onClicked: {
        if (!dragStarted)
            itemClicked();
    }
    onDoubleClicked: {
        if (!dragStarted)
            itemActivated();
    }

    Item {
        id: dragTarget
        visible: false
    }
}
