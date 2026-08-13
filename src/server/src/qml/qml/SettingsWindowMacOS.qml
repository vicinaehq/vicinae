SettingsWindow {
    nativeChrome: true

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "sidebar"
    MacOSWindow.transparentTitlebar: true
    MacOSWindow.followsWindowActiveState: true
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
}
