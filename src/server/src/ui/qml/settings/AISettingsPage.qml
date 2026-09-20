pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    readonly property AISettingsModel model: Settings.aiModel
    readonly property string subroute: {
        const subpage = Settings.currentSubpage;
        const slash = subpage.indexOf("/");
        return slash < 0 ? "" : subpage.slice(slash + 1);
    }

    Binding {
        target: root.model
        property: "selectedProviderId"
        value: root.subroute
    }

    AIProvidersOverview {
        anchors.fill: parent
        visible: !root.model.provider.valid
        model: root.model
        backLabel: Settings.extensionModel.selectedTitle
        onBack: Settings.currentSubpage = ""
        onOpenProvider: id => Settings.currentSubpage = "providers/" + id
        onSetUp: (type, label, icon, allowMultiple) => setupDialog.openFor(type, label, icon, allowMultiple)
    }

    AIProviderView {
        anchors.fill: parent
        visible: root.model.provider.valid
        page: root.model.provider
        onBack: Settings.currentSubpage = "providers"
    }

    AIProviderSetupDialog {
        id: setupDialog
        model: root.model
    }
}
