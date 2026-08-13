import QtQuick
import QtQuick.Controls
import QtQuick.Effects

/// Popover background with a frosted-glass effect for in-scene popups.
/// Assign to a Popup's `background` and bind `popup` to it. Falls back to the
/// regular opaque popover fill when the popup is a native window
/// (Popup.Window) or the host window exposes no `popupBackdrop` item.
Item {
    id: bg

    required property var popup
    property real backgroundOpacity: nativeWindow ? Config.popupOpacity : 1

    readonly property bool nativeWindow: popup.popupType === Popup.Window
    readonly property bool csd: !nativeWindow || Platform.supports("clientSideDecorations")
    readonly property real cornerRadius: csd ? Math.min(Config.borderRounding, 15) : 0
    readonly property Item backdrop: bg.Window.window?.popupBackdrop ?? null
    readonly property bool frosted: !nativeWindow && backdrop !== null
    readonly property int backdropPad: 96

    // bg's origin in backdrop coordinates.
    readonly property point _origin: {
        popup.x;
        popup.y;
        return backdrop ? bg.mapToItem(backdrop, 0, 0) : Qt.point(0, 0);
    }
    // Padded capture area, clamped to the backdrop bounds so the blur never
    // samples transparent void (it darkens into a halo).
    readonly property rect _captureRect: {
        if (!backdrop)
            return Qt.rect(0, 0, 1, 1);
        const x0 = Math.max(0, _origin.x - backdropPad);
        const y0 = Math.max(0, _origin.y - backdropPad);
        const x1 = Math.min(backdrop.width, _origin.x + bg.width + backdropPad);
        const y1 = Math.min(backdrop.height, _origin.y + bg.height + backdropPad);
        return Qt.rect(x0, y0, Math.max(1, x1 - x0), Math.max(1, y1 - y0));
    }

    Rectangle {
        visible: bg.frosted
        anchors.fill: parent
        radius: bg.cornerRadius
        color: Theme.background
    }

    ShaderEffectSource {
        id: backdropSource
        visible: false
        sourceItem: bg.frosted && bg.popup.visible ? bg.backdrop : null
        sourceRect: bg._captureRect
        textureSize: Qt.size(Math.max(1, Math.round(bg._captureRect.width / 16)), Math.max(1, Math.round(bg._captureRect.height / 16)))
    }

    MultiEffect {
        visible: bg.frosted
        x: bg._captureRect.x - bg._origin.x
        y: bg._captureRect.y - bg._origin.y
        width: bg._captureRect.width
        height: bg._captureRect.height
        source: backdropSource
        autoPaddingEnabled: false
        blurEnabled: true
        blur: 1.0
        blurMax: 64

        layer.enabled: bg.frosted
        layer.effect: MultiEffect {
            autoPaddingEnabled: false
            blurEnabled: true
            blur: 1.0
            blurMax: 64
            maskEnabled: true
            maskSource: popupBlurMask
        }
    }

    Item {
        id: popupBlurMask
        width: bg._captureRect.width
        height: bg._captureRect.height
        visible: false
        layer.enabled: true

        Rectangle {
            x: bg._origin.x - bg._captureRect.x
            y: bg._origin.y - bg._captureRect.y
            width: bg.width
            height: bg.height
            radius: bg.cornerRadius
            color: "white"
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: bg.cornerRadius
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, bg.frosted ? 0.85 : bg.backgroundOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, bg.backgroundOpacity)
        border.width: bg.csd ? 1 : 0
    }
}
