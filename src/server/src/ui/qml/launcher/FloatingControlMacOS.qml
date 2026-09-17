pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import Vicinae

Window {
    id: root
    required property Item anchorItem

    readonly property point origin: {
        const window = anchorItem.Window.window;
        if (!window)
            return Qt.point(0, 0);
        window.x;
        window.y;
        window.width;
        window.height;
        // mapToGlobal does not make the binding depend on ancestor geometry.
        for (let item = anchorItem; item; item = item.parent) {
            item.x;
            item.y;
        }
        return anchorItem.mapToGlobal(0, 0);
    }

    function syncPosition() {
        const position = anchorItem.mapToGlobal(0, 0);
        x = Math.round(position.x);
        y = Math.round(position.y);
    }

    onOriginChanged: Qt.callLater(syncPosition)
    onVisibleChanged: {
        if (visible) {
            syncPosition();
            Qt.callLater(syncPosition);
        }
    }

    transientParent: anchorItem.Window.window
    width: anchorItem.width
    height: anchorItem.height
    // Prevent native resize regions from swallowing clicks along the control's edges.
    minimumWidth: anchorItem.width
    maximumWidth: anchorItem.width
    minimumHeight: anchorItem.height
    maximumHeight: anchorItem.height
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
