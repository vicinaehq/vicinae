import QtQuick

SourceBlendRect {
    property bool filled: false
    readonly property bool _enabled: opacity >= 1

    visible: filled
    radius: 8
    backgroundColor: _enabled ? Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity) : "transparent"
    color: Config.withAlpha(Theme.secondaryBackground, Config.windowOpacity)
}
