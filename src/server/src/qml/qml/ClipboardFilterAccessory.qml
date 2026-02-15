import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    readonly property var _options: ["All", "Text", "Images", "Links", "Files"]

    items: [{
        title: "",
        items: _options.map((name, i) => ({ id: i.toString(), displayName: name }))
    }]

    currentItem: {
        var idx = launcher.commandViewHost ? launcher.commandViewHost.currentKindFilter : 0
        return { id: idx.toString(), displayName: _options[idx] }
    }

    onActivated: (item) => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setKindFilter(parseInt(item.id))
    }
}
