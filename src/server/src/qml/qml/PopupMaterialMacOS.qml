import QtQuick

Item {
    MacOSWindow.enabled: Config.blurEnabled
    MacOSWindow.blurEnabled: Config.blurEnabled
    MacOSWindow.cornerRadius: Math.min(Config.borderRounding, 15)
    MacOSWindow.material: Config.windowMaterial === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.borderColor: Theme.divider
    MacOSWindow.borderWidth: Config.borderWidth

    function animateIn(ax, ay) {
        MacOSWindow.animateIn(ax === undefined ? 0.5 : ax, ay === undefined ? 0.5 : ay);
    }
    function animateOut(ax, ay) {
        MacOSWindow.animateOut(ax === undefined ? 0.5 : ax, ay === undefined ? 0.5 : ay);
    }
}
