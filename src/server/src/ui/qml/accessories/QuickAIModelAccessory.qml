import QtQuick

SearchableDropdown {
    compact: true
    minimumWidth: 120
    items: launcher.commandViewHost?.modelSelectorItems ?? []
    currentItem: launcher.commandViewHost?.modelSelectorCurrentItem ?? null
    onActivated: (item) => {
        launcher.commandViewHost?.selectModel(item.id)
    }
}
