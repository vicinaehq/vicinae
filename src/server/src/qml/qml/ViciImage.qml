import QtQuick

Item {
    id: root
    property var source
    property int fillMode: Image.Stretch
    property bool cache: true
    property size sourceSize: Qt.size(-1, -1)

    readonly property int status: staticImg.status
    implicitWidth: staticImg.implicitWidth
    implicitHeight: staticImg.implicitHeight

    Image {
        id: staticImg
        anchors.fill: parent
        visible: !animImg.animated
        fillMode: root.fillMode
        cache: root.cache
        source: {
            var s = root.source
            if (!s) return ""
            if (typeof s === "string") return s
            if (!s.valid) return ""
            if (s.isThemeSensitive) { var _ = Theme.foreground }
            return s.toSource()
        }
        asynchronous: true
        sourceSize.width: root.sourceSize.width >= 0 ? root.sourceSize.width : root.width
        sourceSize.height: root.sourceSize.height >= 0 ? root.sourceSize.height : root.height
        onStatusChanged: {
            if (status === Image.Ready)
                animImg.source = staticImg.source
        }
    }

    ViciAnimatedImage {
        id: animImg
        anchors.fill: parent
        visible: animated
    }
}
