pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 140

    readonly property var _host: Launcher.commandViewHost

    model: _host?.folderFilterModel ?? null

    currentItem: {
        const m = _host?.folderFilterModel;
        const id = _host?.currentFolderFilter;
        if (!m || id === undefined)
            return null;
        return m.itemDataById(id);
    }

    onActivated: item => {
        if (root._host)
            root._host.setFolderFilter(item.id);
    }
}
