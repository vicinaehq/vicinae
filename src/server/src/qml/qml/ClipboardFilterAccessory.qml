import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    readonly property var _host: launcher.commandViewHost

    model: _host?.kindFilterModel ?? null

    currentItem: {
        const m = _host?.kindFilterModel;
        const idx = _host?.currentKindFilter;
        if (!m || idx === undefined)
            return null;
        return m.itemDataById(idx.toString());
    }

    onActivated: item => {
        if (root._host)
            root._host.setKindFilter(parseInt(item.id));
    }
}
