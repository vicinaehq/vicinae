pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

PopupMaterialStyle {
    backgroundComponent: GlassSurfaceMacOS {}
    itemBackgroundComponent: PopupItemBackgroundMacOS {}
    selectedText: Config.blurEnabled ? Theme.foreground : Theme.listItemSelectionFg

    MacOSWindow.enabled: Config.blurEnabled
    MacOSWindow.blurEnabled: Config.blurEnabled
    MacOSWindow.cornerRadius: Math.min(Config.borderRounding, 15)
    MacOSWindow.material: Config.popupMaterial === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
    MacOSWindow.borderWidth: 0

    function animateIn(ax, ay) {
        MacOSWindow.animateIn(ax === undefined ? 0.5 : ax, ay === undefined ? 0.5 : ay);
    }
    function animateOut(ax, ay) {
        MacOSWindow.animateOut(ax === undefined ? 0.5 : ax, ay === undefined ? 0.5 : ay);
    }
}
