pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Window {
    property int shadowPadding: 0
    property LauncherAppearance appearance: LauncherAppearance {}
    readonly property int searchBarOverlap: Launcher.searchVisible && appearance.overlaySearchBar ? appearance.searchBarHeight : 0
    property int statusBarOverlap: 0
    property real statusBarTop: 0
    property Item popupBackdrop: null
}
