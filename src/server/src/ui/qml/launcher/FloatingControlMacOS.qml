pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import Vicinae

Window {
    id: root
    required property Item anchorItem

    readonly property rect anchorGeometry: {
        const window = anchorItem.Window.window;
        if (!window)
            return Qt.rect(0, 0, 0, 0);
        window.x;
        window.y;
        window.width;
        window.height;
        // mapToGlobal does not make the binding depend on ancestor geometry.
        for (let item = anchorItem; item; item = item.parent) {
            item.x;
            item.y;
        }
        const position = anchorItem.mapToGlobal(0, 0);
        return Qt.rect(Math.round(position.x), Math.round(position.y), Math.round(anchorItem.width), Math.round(anchorItem.height));
    }

    function syncGeometry() {
        if (!anchorItem.Window.window)
            return;
        const position = anchorItem.mapToGlobal(0, 0);
        const targetWidth = Math.round(anchorItem.width);
        const targetHeight = Math.round(anchorItem.height);
        // Size constraints can resize a QWindow themselves; admit both sizes before moving it.
        minimumWidth = Math.min(width, targetWidth);
        minimumHeight = Math.min(height, targetHeight);
        maximumWidth = Math.max(width, targetWidth);
        maximumHeight = Math.max(height, targetHeight);
        // Apply size and position together so a right-aligned pill never resizes at its old origin.
        setGeometry(Math.round(position.x), Math.round(position.y), targetWidth, targetHeight);
        // Keep the native resize regions from swallowing clicks along the control's edges.
        minimumWidth = maximumWidth = targetWidth;
        minimumHeight = maximumHeight = targetHeight;
    }

    onAnchorGeometryChanged: Qt.callLater(syncGeometry)
    onVisibleChanged: {
        if (visible) {
            syncGeometry();
            Qt.callLater(syncGeometry);
        }
    }

    transientParent: anchorItem.Window.window
    visible: anchorItem.visible && (transientParent?.visible ?? false) && !Launcher.compacted && !Launcher.hasOverlay && !Launcher.alertModel.visible
    // The launcher stays transparent while AppKit finishes placing it.
    opacity: transientParent?.opacity ?? 0
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    color: "transparent"

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: Config.blurEnabled
    MacOSWindow.cornerRadius: height / 2
    MacOSWindow.material: Config.popupMaterial === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.borderWidth: 0

    MacOSPanel.enabled: true
    MacOSPanel.attachedToParent: true
    MacOSPanel.windowLevel: Launcher.filePicking ? MacOSPanel.Floating : MacOSPanel.Status

    GlassSurfaceMacOS {
        anchors.fill: parent
        radius: height / 2
    }
}
