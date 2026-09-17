pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

OnboardingWindow {
    backgroundColor: MaterialColorsMacOS.tint(Theme.background, Config.windowOpacity)
    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "hud"
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
}
