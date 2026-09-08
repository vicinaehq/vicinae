import QtQuick
import Vicinae

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    model: Launcher.commandViewHost?.dropdownModel ?? null
    currentItem: Launcher.commandViewHost?.dropdownCurrentItem ?? null
    placeholder: Launcher.commandViewHost?.dropdownPlaceholder ?? ""

    onActivated: item => {
        if (Launcher.commandViewHost)
            Launcher.commandViewHost.setDropdownValue(item.id);
    }
}
