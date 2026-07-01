import QtQuick

// Applies non-activating panel behaviour to the popup window it lives in, so a
// completion popup can show without stealing key focus from the field driving
// it. macOS only; loaded via a Loader so the type never resolves elsewhere.
Item {
    MacOSPanel.enabled: true
    MacOSPanel.windowLevel: MacOSPanel.PopUpMenu
}
