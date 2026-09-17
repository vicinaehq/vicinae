pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

QtObject {
    property color windowBackground: Config.withAlpha(Theme.background, Config.windowOpacity)
    property Component dividerComponent: null
    property int searchBarHeight: 60
    property bool searchDividerVisible: true
    property bool overlaySearchBar: false
    property int contentInset: Config.borderWidth
    property bool floatingStatusBar: Config.floatingStatusBar
    property int contentBottomInset: 0

    property int rowInset: 6
    property int rowRadius: 10
    property int rowLeftPadding: 12
    property int rowRightPadding: 12
    property int rowTextSpacing: 6
    property int sectionHeaderHeight: 30
    property color selectedText: Theme.listItemSelectionFg
    property color selectedSecondaryText: Theme.listItemSecondarySelectionFg
    property color hoveredText: Theme.listItemHoverFg
    property color hoveredSecondaryText: Theme.listItemSecondaryHoverFg
    property color gridSelectionOutline: Theme.gridItemSelectionOutline
    property color gridHoverOutline: Theme.gridItemHoverOutline
    property int gridSelectionBorderWidth: 2
    property int gridHoverBorderWidth: 2
    property color gridFill: Config.withAlpha(Theme.gridItemBackground, Config.surfaceOpacity)
    property color gridSelectionFill: gridFill
    property color gridHoverFill: gridFill
    property Component gridContentEffect: null
    property color delegateBackdrop: Config.withAlpha(Theme.background, Config.windowOpacity)
    property color selectionFill: Config.withAlpha(Theme.listItemSelectionBg, Config.surfaceOpacity)
    property color hoverFill: Config.withAlpha(Theme.listItemHoverBg, Config.surfaceOpacity)
}
