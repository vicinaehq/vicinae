import QtQuick

SettingsWindow {
    nativeChrome: true

    headerAccessory: Component {
        SettingsNavButtonsMacOS {}
    }

    minimumHeight: 600
    maximumHeight: 16777215

    MacOSWindow.enabled: true
    MacOSWindow.blurEnabled: true
    MacOSWindow.material: "sidebar"
    MacOSWindow.transparentTitlebar: true
    MacOSWindow.followsWindowActiveState: true
    MacOSWindow.appearance: Theme.isDark ? "dark" : "light"
}
