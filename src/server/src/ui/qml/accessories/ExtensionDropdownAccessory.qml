pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SearchableDropdown {
    id: root
    readonly property ExtensionViewHost host: Launcher.commandViewHost as ExtensionViewHost
    compact: true
    minimumWidth: 100

    model: root.host?.dropdownModel ?? null
    currentItem: root.host?.dropdownCurrentItem ?? null
    placeholder: root.host?.dropdownPlaceholder ?? ""

    onActivated: item => {
        if (root.host)
            root.host.setDropdownValue(item.id);
    }
}
