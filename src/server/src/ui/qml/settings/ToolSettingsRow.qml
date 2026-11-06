pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SettingsRow {
    id: root
    required property var tool
    required property var policyItems
    signal policySelected(string policy)

    label: tool.name
    description: tool.description
    iconSource: tool.iconSource
    controlWidth: 160

    SearchableDropdown {
        width: parent.width
        items: root.policyItems
        currentItem: root.policyItems[0].items.find(item => item.id === root.tool.policy) ?? null
        onActivated: item => root.policySelected(item.id)
    }
}
