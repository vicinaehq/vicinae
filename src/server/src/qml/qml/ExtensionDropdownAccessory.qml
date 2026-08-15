import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    model: launcher.commandViewHost ? launcher.commandViewHost.dropdownModel : null
    currentItem: launcher.commandViewHost ? launcher.commandViewHost.dropdownCurrentItem : null
    placeholder: launcher.commandViewHost ? launcher.commandViewHost.dropdownPlaceholder : ""

    onActivated: item => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setDropdownValue(item.id);
    }
}
