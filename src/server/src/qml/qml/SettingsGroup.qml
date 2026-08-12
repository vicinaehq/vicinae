import QtQuick
import QtQuick.Layouts

// Grouped "inset card" container for settings rows.
SourceBlendRect {
    id: root
    default property alias content: inner.data

    Layout.fillWidth: true
    implicitHeight: inner.implicitHeight

    radius: Style.cardRadius
    backgroundColor: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
    color: Config.withAlpha(Theme.secondaryBackground, Config.windowOpacity)
    borderColor: Config.withAlpha(Theme.divider, Config.windowOpacity)
    borderWidth: Style.cardBorderWidth

    ColumnLayout {
        id: inner
        width: parent.width
        spacing: 0
    }
}
