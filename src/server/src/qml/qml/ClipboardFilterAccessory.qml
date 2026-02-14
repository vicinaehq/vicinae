import QtQuick

SearchableDropdown {
    options: ["All", "Text", "Images", "Links", "Files"]
    minimumWidth: 100
    currentIndex: launcher.commandViewHost ? launcher.commandViewHost.currentKindFilter : 0
    onActivated: function(index) {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setKindFilter(index)
    }
}
