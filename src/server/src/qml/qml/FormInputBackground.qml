import QtQuick

SourceBlendRect {
    property bool filled: false

    visible: filled
    radius: 8
    backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
    color: Config.withAlpha(Theme.secondaryBackground, Config.windowOpacity)
}
