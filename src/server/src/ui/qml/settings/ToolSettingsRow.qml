pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SettingsRow {
    id: root
    required property var tool
    signal toggled(bool checked)

    label: tool.name
    description: tool.description
    iconSource: tool.iconSource
    controlWidth: 34
    enabled: tool.providerEnabled
    opacity: enabled ? 1 : 0.5

    SettingsToggle {
        checked: root.tool.isEnabled
        onToggled: checked => root.toggled(checked)
    }
}
