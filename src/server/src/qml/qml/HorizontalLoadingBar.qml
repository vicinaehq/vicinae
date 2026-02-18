import QtQuick

Rectangle {
    id: root
    property bool loading: false
    color: Theme.divider

    property bool _active: false

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
        duration: (root.width + bar.width) / 10 * 10  // match C++: 10px per 10ms
        loops: Animation.Infinite
        running: root._active
    }

    Timer {
        id: debounce
        interval: 100
        onTriggered: {
            if (root.loading) {
                bar.x = -bar.width
                root._active = true
            } else {
                root._active = false
            }
        }
    }

    onLoadingChanged: debounce.restart()
}
