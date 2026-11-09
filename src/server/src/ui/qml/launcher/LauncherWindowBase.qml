pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Window {
    property int shadowPadding: 0
    property LauncherAppearance appearance: LauncherAppearance {}
    property bool headerVisible: Launcher.searchVisible
    property real headerHeight: appearance.searchBarHeight
    readonly property real headerOverlap: headerVisible && appearance.overlaySearchBar ? headerHeight : 0
    property int statusBarOverlap: 0
    property real statusBarTop: 0
    property Item popupBackdrop: null
}
