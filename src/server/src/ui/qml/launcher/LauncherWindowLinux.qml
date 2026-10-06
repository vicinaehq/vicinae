pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

LauncherWindow {
    id: root

    appearance: LauncherAppearanceMacOS {}
    searchBarComponent: SearchBarMacOS {
        commandView: root.commandStack
    }
    contentEffect: ScrollFadeMacOS {
        topInset: root.searchBarOverlap
        bottomInset: root.statusBarOverlap
    }

    onAboutToShow: Launcher.prepareShow()
    onShown: Launcher.finalizeShow()
}
