pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

SearchableDropdown {
    id: root
    compact: true
    minimumWidth: 120

    readonly property var _host: Launcher.commandViewHost

    items: _host?.modelSelectorItems ?? []
    currentItem: _host?.modelSelectorCurrentItem ?? null

    onActivated: item => {
        if (root._host)
            root._host.selectModel(item.id);
    }
}
