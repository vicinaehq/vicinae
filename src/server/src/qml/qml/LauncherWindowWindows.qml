import QtQuick

LauncherWindow {
    id: root
    nativeChrome: true
    cornerRadius: WindowsWindow.nativeCornerRadius
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    height: _contentH
    minimumHeight: _contentH
    maximumHeight: _contentH

    WindowsWindow.enabled: true
    WindowsWindow.blurEnabled: blurEnabled
    WindowsWindow.appearance: Theme.isDark ? "dark" : "light"
    WindowsWindow.borderColor: Theme.mainWindowBorder

    // Task View's dismissal briefly reclaims foreground right after a summon:
    // take focus back within the grace period instead of closing, paced
    // through a timer — synchronous re-requests ping-pong with the shell.
    property real _shownAt: 0
    property int _activationRetries: 0

    Timer {
        id: reactivateTimer
        interval: 150
        onTriggered: if (root.visible && !root.active) root.requestActivate()
    }

    onVisibleChanged: if (visible) {
        _shownAt = Date.now();
        _activationRetries = 0;
    }

    onActiveChanged: {
        if (active || !visible)
            return;
        if (Date.now() - _shownAt < 600) {
            if (_activationRetries < 3) {
                _activationRetries++;
                reactivateTimer.restart();
            }
        } else {
            Nav.closeWindow();
        }
    }
}
