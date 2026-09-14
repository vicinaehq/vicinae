pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Item {
    id: root

    readonly property AISettingsModel model: Settings.aiModel

    // The open provider is a settings sub-route so the history arrows walk through it.
    Binding {
        target: root.model
        property: "selectedProviderId"
        value: Settings.currentSubpage
    }

    AIProvidersOverview {
        anchors.fill: parent
        visible: !root.model.provider.valid
        model: root.model
        onOpenProvider: id => Settings.currentSubpage = id
        onSetUp: (type, label, icon, allowMultiple) => setupDialog.openFor(type, label, icon, allowMultiple)
    }

    AIProviderView {
        anchors.fill: parent
        visible: root.model.provider.valid
        page: root.model.provider
        onBack: Settings.currentSubpage = ""
    }

    AIProviderSetupDialog {
        id: setupDialog
        model: root.model
    }
}
