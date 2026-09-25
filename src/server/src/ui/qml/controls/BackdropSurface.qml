pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Effects
import Vicinae

Rectangle {
    id: root
    required property Item sourceItem
    property rect sourceRect: Qt.rect(0, 0, width, height)
    property real tintOpacity: 0.94
    readonly property bool frosted: Config.blurEnabled && sourceItem !== null
    color: Theme.background
    clip: true

    ShaderEffectSource {
        id: backdrop
        visible: false
        sourceItem: root.frosted && root.visible ? root.sourceItem : null
        sourceRect: root.sourceRect
        textureSize: Qt.size(Math.max(1, Math.ceil(root.sourceRect.width / 4)), Math.max(1, Math.ceil(root.sourceRect.height / 4)))
    }

    MultiEffect {
        anchors.fill: parent
        visible: root.frosted
        source: backdrop
        autoPaddingEnabled: false
        blurEnabled: true
        blur: 1
        blurMax: 16
    }

    Rectangle {
        anchors.fill: parent
        color: Config.withAlpha(root.color, root.frosted ? root.tintOpacity : 1)
    }
}
