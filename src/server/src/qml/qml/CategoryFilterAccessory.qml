import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    readonly property var _options: launcher.commandViewHost ? launcher.commandViewHost.categoryFilterOptions : [qsTr("All")]

    items: [
        {
            title: "",
            items: _options?.map((name, i) => ({
                        id: i.toString(),
                        displayName: name
                    })) ?? []
        }
    ]

    currentItem: {
        var idx = launcher.commandViewHost ? launcher.commandViewHost.currentCategoryFilter : 0;
        return {
            id: idx?.toString() ?? 'unknown',
            displayName: _options?.[idx] ?? ''
        };
    }

    onActivated: item => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setCategoryFilter(parseInt(item.id));
    }
}
