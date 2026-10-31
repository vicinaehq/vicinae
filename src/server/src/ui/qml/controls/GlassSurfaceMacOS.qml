import QtQuick
import Vicinae

Rectangle {
    radius: 15
    color: Config.blurEnabled ? Config.withAlpha(Theme.foreground, Theme.isDark ? 0.08 : 0.04) : Theme.popoverBackground
    border.width: 1
    border.color: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.1 : 0.08)
}
