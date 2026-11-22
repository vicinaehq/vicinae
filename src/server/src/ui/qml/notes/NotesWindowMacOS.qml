import QtQuick
import Vicinae

NotesWindow {
    id: root
    nativeChrome: true
    supportsKeepOnTop: true
    readonly property rect windowButtons: MacOSWindow.titlebarButtonsRect
    toolbarCenterY: windowButtons.height > 0 ? windowButtons.y + windowButtons.height / 2 : 20
    toolbarLeftInset: windowButtons.width > 0 ? windowButtons.x + windowButtons.width + 14 : 84
    headerHeight: Math.max(40, toolbarCenterY * 2)
    translucentBackground: Config.blurEnabled
    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: root.translucentBackground
    MacOSWindow.material: "sidebar"
    MacOSWindow.transparentTitlebar: true
    MacOSWindow.compactToolbar: true
    MacOSWindow.titlebarControls: root.titlebarControls
    MacOSWindow.followsWindowActiveState: true
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
}
