import QtQuick
import Vicinae

ChatWindow {
    id: root
    nativeChrome: true
    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "sidebar"
    MacOSWindow.transparentTitlebar: true
    MacOSWindow.compactToolbar: true
    MacOSWindow.titlebarControls: root.titlebarControls
    MacOSWindow.followsWindowActiveState: true
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
}
