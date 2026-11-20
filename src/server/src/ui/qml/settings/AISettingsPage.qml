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
        onAddProvider: picker.openPicker()
    }

    AIProviderPicker {
        id: picker
        model: root.model
        onChosen: (type, label, icon, allowMultiple, needsSetup) => {
            if (needsSetup)
                setupDialog.openFor(type, label, icon, allowMultiple);
            else
                root.model.addProvider(type, {});
        }
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
