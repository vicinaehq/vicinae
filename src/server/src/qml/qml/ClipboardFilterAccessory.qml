import QtQuick

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 100

    model: launcher.commandViewHost ? launcher.commandViewHost.kindFilterModel : null

    currentItem: {
        const host = launcher.commandViewHost;
        if (!host)
            return null;
        return host.kindFilterModel.itemDataById(host.currentKindFilter.toString());
    }

    onActivated: item => {
        if (launcher.commandViewHost)
            launcher.commandViewHost.setKindFilter(parseInt(item.id));
    }
}
