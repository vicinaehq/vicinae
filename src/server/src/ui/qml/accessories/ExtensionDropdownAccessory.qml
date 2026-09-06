import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    model: launcher.commandViewHost?.dropdownModel ?? null
    currentItem: launcher.commandViewHost?.dropdownCurrentItem ?? null
    placeholder: launcher.commandViewHost?.dropdownPlaceholder ?? ""

    onActivated: item => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setDropdownValue(item.id);
    }
}
