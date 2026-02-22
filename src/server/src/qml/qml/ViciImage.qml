import QtQuick

Item {
    id: root
    property var source
    property int fillMode: Image.PreserveAspectFit
    property bool cache: true
    property size sourceSize: Qt.size(-1, -1)

    readonly property int status: staticImg.status
    implicitWidth: root.sourceSize.width >= 0
        ? (animImg.animated ? animImg.implicitWidth : staticImg.implicitWidth) : 0
    implicitHeight: root.sourceSize.height >= 0
        ? (animImg.animated ? animImg.implicitHeight : staticImg.implicitHeight) : 0

    property bool _errored: false
    onSourceChanged: _errored = false

    readonly property string _resolvedSource: {
        var s = root.source
        if (!s) return ""
        if (typeof s === "string") return s
        if (!s.valid) return ""
        if (s.isThemeSensitive) { var _ = Theme.foreground }
        return s.toSource()
    }

    readonly property string _fallbackSource:
        "image://vicinae/builtin:question-mark-circle?fg=" + Theme.foreground

    Image {
        id: staticImg
        anchors.fill: parent
        visible: !animImg.animated
        fillMode: root.fillMode
        cache: root.cache
        source: root._errored ? root._fallbackSource : root._resolvedSource
        asynchronous: true
        sourceSize.width: root.sourceSize.width >= 0 ? root.sourceSize.width : root.width
        sourceSize.height: root.sourceSize.height >= 0 ? root.sourceSize.height : root.height
        onStatusChanged: {
            if (status === Image.Error && !root._errored) {
                console.warn("ViciImage: failed to load", root._resolvedSource)
                root._errored = true
            } else if (status === Image.Ready && !root._errored)
                animImg.source = staticImg.source
        }
    }

    ViciAnimatedImage {
        id: animImg
        anchors.fill: parent
        visible: animated
    }
}
