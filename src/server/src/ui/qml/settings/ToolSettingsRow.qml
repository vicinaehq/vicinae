pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

SettingsRow {
    id: root
    required property var tool
    required property var policyItems
    property bool expanded: false
    signal policySelected(string policy)
    signal preferencesRequested

    label: tool.name
    description: tool.description
    iconSource: tool.iconSource
    controlWidth: tool.hasPreferences ? 200 : 160

    RowLayout {
        width: parent.width
        spacing: 8

        ViciButton {
            visible: root.tool.hasPreferences
            accessibleName: qsTr("Preferences")
            iconSource: Img.icon(root.expanded ? BuiltinIcon.ChevronDownSmall : BuiltinIcon.ChevronRightSmall)
            onClicked: root.preferencesRequested()
        }

        SearchableDropdown {
            Layout.fillWidth: true
            items: root.policyItems
            currentItem: root.policyItems[0].items.find(item => item.id === root.tool.policy) ?? null
            onActivated: item => root.policySelected(item.id)
        }
    }
}
