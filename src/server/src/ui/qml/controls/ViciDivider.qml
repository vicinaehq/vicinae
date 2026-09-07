import QtQuick

SourceBlendRect {
    property bool vertical: false
    implicitWidth: vertical ? 1 : -1
    implicitHeight: vertical ? -1 : 1
    color: Config.withAlpha(Theme.divider, Config.windowOpacity)
}
