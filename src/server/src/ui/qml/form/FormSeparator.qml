import QtQuick
import QtQuick.Layouts
import Vicinae

Rectangle {
    Layout.fillWidth: true
    Layout.topMargin: 5
    Layout.bottomMargin: 5
    implicitHeight: 1
    color: Config.withAlpha(Theme.divider, Config.windowOpacity)
}
