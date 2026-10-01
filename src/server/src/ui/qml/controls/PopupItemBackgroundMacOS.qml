pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SourceBlendRect {
    readonly property PopupItemBackground control: parent as PopupItemBackground
    radius: control?.radius ?? 0
    backgroundColor: Config.blurEnabled ? "transparent" : Config.withAlpha(Theme.popoverBackground, control?.backgroundOpacity ?? 0)
    color: {
        if (Config.blurEnabled)
            return Config.withAlpha(Theme.foreground, control?.selected ? (Theme.isDark ? 0.14 : 0.1) : control?.hovered ? 0.07 : 0);
        const fill = control?.selected ? Theme.listItemSelectionBg : control?.hovered ? Theme.listItemHoverBg : Theme.popoverBackground;
        const alpha = control?.selected || control?.hovered ? Config.popupSurfaceOpacity : control?.backgroundOpacity ?? 0;
        return Config.withAlpha(fill, alpha);
    }
}
