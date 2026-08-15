import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    model: launcher.commandViewHost ? launcher.commandViewHost.categoryFilterModel : null

    currentItem: {
        const host = launcher.commandViewHost;
        if (!host)
            return null;
        return host.categoryFilterModel.itemDataById(host.currentCategoryFilter.toString());
    }

    onActivated: item => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setCategoryFilter(parseInt(item.id));
    }
}
