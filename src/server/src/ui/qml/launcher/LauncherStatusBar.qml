pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Effects
import Vicinae

Item {
    id: root
    required property Item backdrop
    required property int windowRadius
    required property int windowHeight
    required property int windowWidth
    readonly property alias popupAnchor: footer
    implicitHeight: 41 + Config.borderWidth
    clip: true

    readonly property int backdropPad: 64

    ShaderEffectSource {
        id: statusBarBackdrop
        visible: false
        sourceItem: Config.floatingStatusBar ? root.backdrop : null
        sourceRect: Qt.rect(-Config.borderWidth, root.backdrop.height - (root.height - Config.borderWidth) - root.backdropPad, root.width, root.height + root.backdropPad)
        textureSize: Qt.size(Math.max(1, Math.round(root.width / 10)), Math.max(1, Math.round((root.height + root.backdropPad) / 10)))
    }

    MultiEffect {
        visible: Config.floatingStatusBar
        y: -root.backdropPad
        width: root.width
        height: root.height + root.backdropPad
        source: statusBarBackdrop
        autoPaddingEnabled: false
        blurEnabled: true
        blur: 1.0
        blurMax: 64

        layer.enabled: true
        layer.effect: MultiEffect {
            autoPaddingEnabled: false
            blurEnabled: true
            blur: 1.0
            blurMax: 64
            maskEnabled: true
            maskSource: statusBarBlurMask
        }
    }

    Rectangle {
        width: root.windowWidth
        height: root.windowHeight
        anchors.bottom: parent.bottom
        radius: root.windowRadius
        color: Config.withAlpha(Theme.statusBarBackground, (Config.floatingStatusBar ? 0.78 : 1.0) * Config.windowOpacity)
    }

    ViciDivider {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Config.borderWidth
        anchors.rightMargin: Config.borderWidth
    }

    Footer {
        id: footer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: Config.borderWidth
        anchors.rightMargin: Config.borderWidth
        anchors.bottomMargin: Config.borderWidth
        height: 40
    }

    Item {
        id: statusBarBlurMask
        width: root.width
        height: root.height + root.backdropPad
        visible: false
        layer.enabled: true

        Rectangle {
            width: root.windowWidth
            height: root.windowHeight
            anchors.bottom: parent.bottom
            radius: root.windowRadius
            color: "white"
        }
    }
}
