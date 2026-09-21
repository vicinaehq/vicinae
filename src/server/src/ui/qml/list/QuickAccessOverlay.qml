pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Effects
import Vicinae

Item {
    id: root
    required property ListView list
    property real topInset: 0
    property real bottomInset: 0

    anchors.fill: root.list
    anchors.topMargin: root.topInset
    clip: true
    visible: opacity > 0
    opacity: active ? 1 : 0

    readonly property bool active: Launcher.commandHeld && !Launcher.alertModel.visible && !Launcher.actionPanel.open && !Launcher.footerPanel.open && !Launcher.hasOverlay

    Behavior on opacity {
        NumberAnimation {
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

    ShortcutBadge {
        id: shortcutMetrics
        visible: false
        tokens: Keyboard.tokens(Qt.Key_1, Qt.ControlModifier)
    }

    Rectangle {
        id: quickAccessBackdrop
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: shortcutMetrics.implicitWidth + 40

        readonly property real topFade: Math.min(32, height / 2)
        readonly property real bottomFade: Math.min(Math.max(32, root.bottomInset), height / 2)

        layer.enabled: root.visible
        layer.effect: MultiEffect {
            autoPaddingEnabled: false
            maskEnabled: true
            maskThresholdMin: 0.5
            maskSpreadAtMin: 1
            maskSource: quickAccessMask
        }

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop {
                position: 0
                color: Config.withAlpha(Theme.background, 0)
            }
            GradientStop {
                position: 0.35
                color: Config.withAlpha(Theme.background, 0.72)
            }
            GradientStop {
                position: 1
                color: Config.withAlpha(Theme.background, 0.72)
            }
        }
    }

    Rectangle {
        id: quickAccessMask
        width: 1
        height: Math.max(1, quickAccessBackdrop.height)
        visible: false
        layer.enabled: root.visible
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "transparent"
            }
            GradientStop {
                position: quickAccessBackdrop.topFade / quickAccessMask.height
                color: "white"
            }
            GradientStop {
                position: 1 - quickAccessBackdrop.bottomFade / quickAccessMask.height
                color: "white"
            }
            GradientStop {
                position: 1
                color: "transparent"
            }
        }
    }

    // Follow only instantiated rows, including delegates recycled by the list.
    Repeater {
        model: root.list.contentItem.children

        ShortcutBadge {
            required property Item modelData
            readonly property SelectableDelegate row: (modelData as Loader)?.item as SelectableDelegate
            readonly property int shortcutIndex: row?.quickAccessIndex ?? -1

            visible: shortcutIndex >= 0 && modelData.visible
            anchors.right: root.right
            anchors.rightMargin: 12
            y: modelData.y - root.list.contentY - root.topInset + (modelData.height - height) / 2
            tokens: Keyboard.tokens(shortcutIndex === 9 ? Qt.Key_0 : Qt.Key_1 + shortcutIndex, Qt.ControlModifier)
            surfaceColor: Qt.tint(Theme.background, Config.withAlpha(contentColor, 0.08))
            transform: Translate {
                x: root.active ? 0 : 8

                Behavior on x {
                    NumberAnimation {
                        duration: 120
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }
    }
}
