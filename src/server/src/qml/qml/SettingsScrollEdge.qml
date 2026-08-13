import QtQuick
import QtQuick.Effects

// Blurs the strip of flickable content just scrolled out of its clipped viewport,
// without painting over whatever sits behind this item.
Item {
    id: root

    required property var flickable
    property real edgeHeight: height
    property real ghostOpacity: 0.45
    property bool active: true

    ShaderEffectSource {
        id: backdrop
        visible: false
        live: true
        width: root.width
        height: root.edgeHeight
        sourceItem: root.active ? root.flickable.contentItem : null
        sourceRect: Qt.rect(0, root.flickable.contentY - root.edgeHeight, root.width, root.edgeHeight)
    }

    MultiEffect {
        visible: root.active
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.edgeHeight
        opacity: root.ghostOpacity
        source: backdrop
        blurEnabled: true
        blur: 1.0
        blurMax: 28
    }
}
