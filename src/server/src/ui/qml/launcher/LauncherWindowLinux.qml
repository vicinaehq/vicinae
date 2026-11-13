pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

LauncherWindow {
    id: root

    appearance: LauncherAppearanceMacOS {}
    searchBarComponent: SearchBarMacOS {
        commandView: root.commandView
    }
    statusBarComponent: LauncherStatusBarLinux {}
    contentEffect: ScrollFadeMacOS {
        topInset: root.searchBarOverlap
        bottomInset: root.statusBarOverlap
    }
}
