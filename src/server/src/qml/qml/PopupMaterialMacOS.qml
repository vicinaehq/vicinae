import QtQuick

Item {
    MacOSWindow.enabled: Config.blurEnabled
    MacOSWindow.blurEnabled: Config.blurEnabled
    MacOSWindow.cornerRadius: Math.min(Config.borderRounding, 15)
    MacOSWindow.material: Config.windowMaterial === "liquid_glass" ? "liquidGlass" : "hud"
    MacOSWindow.borderColor: Theme.divider
    MacOSWindow.borderWidth: Config.borderWidth
}
