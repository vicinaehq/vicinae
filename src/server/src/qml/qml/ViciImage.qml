import QtQuick

Item {
    id: root
    property var source
    property int fillMode: Image.PreserveAspectFit
    property bool cache: true
    property size sourceSize: Qt.size(-1, -1)

    readonly property int status: staticImg.status
    implicitWidth: root.sourceSize.width >= 0 ? (animImg.animated ? animImg.implicitWidth : staticImg.implicitWidth) : 0
    implicitHeight: root.sourceSize.height >= 0 ? (animImg.animated ? animImg.implicitHeight : staticImg.implicitHeight) : 0

    readonly property int _effectiveW: root.sourceSize.width >= 0 ? root.sourceSize.width : root.width
    readonly property int _effectiveH: root.sourceSize.height >= 0 ? root.sourceSize.height : root.height
    readonly property bool _hasValidSize: _effectiveW > 0 && _effectiveH > 0

    readonly property string _resolvedSource: {
        var s = root.source;
        if (!s)
            return "";
        if (typeof s === "string")
            return s;
        if (!s.valid)
            return "";
        if (s.isThemeSensitive) {
            var _ = Theme.foreground;
        }
        return s.toSource();
    }

    Image {
        id: staticImg
        anchors.fill: parent
        visible: !animImg.animated
        fillMode: root.fillMode
        cache: root.cache
        source: root._hasValidSize ? root._resolvedSource : ""
        asynchronous: true
        mipmap: true
        sourceSize.width: root._effectiveW
        sourceSize.height: root._effectiveH
        onStatusChanged: {
            if (status === Image.Ready)
                animImg.source = staticImg.source;
        }
    }

    ViciAnimatedImage {
        id: animImg
        anchors.fill: parent
        visible: animated
    }
}
