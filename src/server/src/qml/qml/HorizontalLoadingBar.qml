import QtQuick

Item {
    id: root
    property bool loading: false
    clip: true

    property bool _active: false

    ViciDivider {
        anchors.fill: parent
    }

    Rectangle {
        id: bar
        width: 50
        height: parent.height
        y: 0
        visible: root._active
        color: Theme.loadingBar
    }

    NumberAnimation {
        id: slideAnimation
        target: bar
        property: "x"
        from: -bar.width
        to: root.width
        duration: (root.width + bar.width) / 10 * 10
        loops: Animation.Infinite
        running: root._active
    }

    Timer {
        id: debounce
        interval: 100
        onTriggered: {
            if (root.loading) {
                bar.x = -bar.width;
                root._active = true;
            } else {
                root._active = false;
            }
        }
    }

    onLoadingChanged: debounce.restart()
}
