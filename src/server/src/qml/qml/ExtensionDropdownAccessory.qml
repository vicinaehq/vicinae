import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    items: launcher.commandViewHost ? launcher.commandViewHost.dropdownItems : []
    currentItem: launcher.commandViewHost ? launcher.commandViewHost.dropdownCurrentItem : null
    placeholder: launcher.commandViewHost ? launcher.commandViewHost.dropdownPlaceholder : ""

    onActivated: (item) => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setDropdownValue(item.id)
    }
}
