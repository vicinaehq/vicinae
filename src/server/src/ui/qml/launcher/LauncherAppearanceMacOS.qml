pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

LauncherAppearance {
    windowBackground: MaterialColorsMacOS.tint(Theme.background, Config.windowOpacity)
    dividerComponent: ContentDividerMacOS {}
    searchDividerVisible: false
    overlaySearchBar: true
    contentInset: 0
    floatingStatusBar: true
    contentBottomInset: 24
    rowInset: 8
    rowRadius: 10
    rowLeftPadding: 14
    rowRightPadding: 16
    rowTextSpacing: 10
    sectionHeaderHeight: 24
    gridSelectionOutline: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.72 : 0.5)
    gridHoverOutline: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.28 : 0.22)
    gridSelectionBorderWidth: 2
    gridFill: Config.withAlpha(Qt.tint(Theme.gridItemBackground, Config.withAlpha(Theme.foreground, Theme.isDark ? 0.07 : 0.02)), Config.surfaceOpacity)
    gridContentEffect: RoundedClipMacOS {}
    delegateBackdrop: "transparent"
    selectionFill: Config.withAlpha(Theme.listItemSelectionBg, Config.surfaceOpacity)
    hoverFill: Config.withAlpha(Theme.foreground, Theme.isDark ? 0.055 : 0.035)
}
