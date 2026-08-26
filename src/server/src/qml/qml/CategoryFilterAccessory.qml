import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    readonly property var _host: launcher.commandViewHost

    model: _host?.categoryFilterModel ?? null

    currentItem: {
        const m = _host?.categoryFilterModel;
        const idx = _host?.currentCategoryFilter;
        if (!m || idx === undefined)
            return null;
        return m.itemDataById(idx.toString());
    }

    onActivated: item => {
        if (root._host)
            root._host.setCategoryFilter(parseInt(item.id));
    }
}
