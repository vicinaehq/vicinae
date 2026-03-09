import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property var model: settings.aiModel
    property int selectedRow: -1
    property int _refreshKey: 0

    readonly property var _dropdownItems: {
        const types = root.model.availableTypes;
        const items = [];
        for (let i = 0; i < types.length; i++) {
            const t = types[i];
            items.push({
                id: t.type,
                displayName: t.label,
                iconSource: "image://vicinae/builtin:" + t.icon + "?fg=" + Theme.foreground
            });
        }
        return [
            {
                title: "",
                items: items
            }
        ];
    }

    Connections {
        target: root.model
        function onProvidersChanged() {
            root._refreshKey++;
            if (root.selectedRow >= root.model.rowCount())
                root.selectedRow = -1;
        }
        function onModelsConfigChanged() {
            root._refreshKey++;
        }
    }

    // ── List view ──
    Flickable {
        id: listFlick
        anchors.fill: parent
        visible: root.selectedRow === -1
        contentWidth: width
        contentHeight: listCol.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ViciScrollBar {
            policy: listFlick.contentHeight > listFlick.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: listCol
            width: listFlick.width
            spacing: 0

            readonly property real contentWidth: Math.min(width, 680)
            readonly property real sideMargin: (width - contentWidth) / 2

            Item {
                implicitHeight: 16
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: listCol.sideMargin + 20
                Layout.rightMargin: listCol.sideMargin + 20
                spacing: 8

                Text {
                    text: "Providers"
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    font.bold: true
                    Layout.fillWidth: true
                }

                SearchableDropdown {
                    compact: true
                    placeholder: "Add Provider"
                    items: root._dropdownItems
                    minimumWidth: 120
                    onActivated: item => {
                        addModal._providerType = item.id;
                        addModal._providerLabel = item.displayName;
                        addModal._providerIcon = item.iconSource;
                        addModal._reset();
                        addModal.open();
                    }
                }
            }

            Item {
                implicitHeight: 12
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: listCol.sideMargin + 20
                Layout.rightMargin: listCol.sideMargin + 20
                height: 1
                color: Theme.divider
            }

            // Empty state
            Item {
                visible: !root.model.hasProviders
                Layout.fillWidth: true
                Layout.preferredHeight: 140

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    ViciImage {
                        source: Img.builtin("computer-chip").withFillColor(Theme.textMuted)
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        Layout.alignment: Qt.AlignHCenter
                        opacity: 0.5
                    }

                    Text {
                        text: "No AI providers configured"
                        color: Theme.textMuted
                        font.pointSize: Theme.regularFontSize
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "Add a provider to get started with AI features."
                        color: Theme.textPlaceholder
                        font.pointSize: Theme.smallerFontSize
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // Provider rows
            Repeater {
                model: root.model

                delegate: Column {
                    id: listDelegate
                    Layout.fillWidth: true

                    required property int index
                    required property string providerId
                    required property string typeLabel
                    required property string icon

                    Rectangle {
                        width: parent.width
                        height: 44
                        color: listDelegateHover.hovered ? Theme.listItemHoverBg : "transparent"

                        HoverHandler {
                            id: listDelegateHover
                        }
                        TapHandler {
                            onTapped: root.selectedRow = listDelegate.index
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: listCol.sideMargin + 20
                            anchors.rightMargin: listCol.sideMargin + 20
                            spacing: 10

                            ViciImage {
                                source: Img.builtin(listDelegate.icon).withFillColor(Theme.textMuted)
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                            }

                            Text {
                                text: listDelegate.providerId
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Text {
                                text: listDelegate.typeLabel
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }

                            ViciImage {
                                source: Img.builtin("chevron-right-small").withFillColor(Theme.textMuted)
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: Theme.divider
                    }
                }
            }

            Item {
                implicitHeight: 24
            }
        }
    }

    // ── Detail view ──
    Flickable {
        id: detailFlick
        anchors.fill: parent
        visible: root.selectedRow >= 0
        contentWidth: width
        contentHeight: detailCol.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ViciScrollBar {
            policy: detailFlick.contentHeight > detailFlick.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        readonly property var details: {
            void root._refreshKey;
            if (root.selectedRow < 0)
                return ({});
            return root.model.providerDetails(root.selectedRow);
        }

        readonly property var providerModels: {
            void root._refreshKey;
            if (root.selectedRow < 0)
                return [];
            return root.model.modelsForProvider(root.selectedRow);
        }

        ColumnLayout {
            id: detailCol
            width: detailFlick.width
            spacing: 0

            readonly property real contentWidth: Math.min(width, 680)
            readonly property real sideMargin: (width - contentWidth) / 2

            Item {
                implicitHeight: 16
            }

            // Breadcrumb
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
                spacing: 0

                Rectangle {
                    implicitWidth: backRow.implicitWidth + 12
                    implicitHeight: 28
                    radius: 6
                    color: breadcrumbHover.hovered ? Theme.listItemHoverBg : "transparent"

                    RowLayout {
                        id: backRow
                        anchors.centerIn: parent
                        spacing: 4

                        ViciImage {
                            source: Img.builtin("chevron-right-small").withFillColor(Theme.textMuted)
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            rotation: 180
                        }

                        Text {
                            text: "Providers"
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                        }
                    }

                    HoverHandler {
                        id: breadcrumbHover
                        cursorShape: Qt.PointingHandCursor
                    }
                    TapHandler {
                        onTapped: root.selectedRow = -1
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Item {
                implicitHeight: 16
            }

            // Provider header
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
                spacing: 12

                ViciImage {
                    source: Img.builtin(detailFlick.details.icon ?? "computer-chip").withFillColor(Theme.foreground)
                    Layout.preferredWidth: 28
                    Layout.preferredHeight: 28
                    Layout.alignment: Qt.AlignTop
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: detailFlick.details.providerId ?? ""
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize + 1
                        font.bold: true
                    }

                    Text {
                        visible: (detailFlick.details.description ?? "") !== ""
                        text: detailFlick.details.description ?? ""
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }

            Item {
                implicitHeight: 24
            }

            // ── Configuration card ──
            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
                implicitHeight: configCardCol.implicitHeight + 32
                radius: 8
                color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.6)
                border.color: Theme.divider
                border.width: 1

                ColumnLayout {
                    id: configCardCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                    spacing: 12

                    Text {
                        text: "Configuration"
                        color: Theme.foreground
                        font.pointSize: Theme.smallerFontSize
                        font.bold: true
                        font.capitalization: Font.AllUppercase
                        opacity: 0.6
                    }

                    // Ollama config
                    ColumnLayout {
                        visible: (detailFlick.details.type ?? "") === "ollama"
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: "Server URL"
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                        }

                        FormTextInput {
                            Layout.fillWidth: true
                            text: detailFlick.details.url ?? ""
                            placeholder: "http://localhost:11434"
                            onAccepted: root.model.setField(root.selectedRow, "url", text.trim())
                            onEditingChanged: {
                                if (!editing)
                                    root.model.setField(root.selectedRow, "url", text.trim());
                            }
                        }
                    }

                    // Mistral config
                    ColumnLayout {
                        visible: (detailFlick.details.type ?? "") === "mistral"
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: "API Key"
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                        }

                        FormSecretInput {
                            text: detailFlick.details.apiKey ?? ""
                            placeholder: "sk-..."
                            onAccepted: root.model.setField(root.selectedRow, "apiKey", text.trim())
                            onEditingChanged: {
                                if (!editing)
                                    root.model.setField(root.selectedRow, "apiKey", text.trim());
                            }
                        }
                    }
                }
            }

            Item {
                implicitHeight: 24
            }

            // ── Models section ──
            Text {
                text: "Models"
                color: Theme.foreground
                font.pointSize: Theme.smallerFontSize
                font.bold: true
                font.capitalization: Font.AllUppercase
                opacity: 0.6
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
            }

            Item {
                implicitHeight: 8
            }

            // Models card
            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
                implicitHeight: modelsCardCol.implicitHeight
                radius: 8
                color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.6)
                border.color: Theme.divider
                border.width: 1
                clip: true

                ColumnLayout {
                    id: modelsCardCol
                    width: parent.width
                    spacing: 0

                    // Empty model state
                    Item {
                        visible: detailFlick.providerModels.length === 0
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60

                        Text {
                            anchors.centerIn: parent
                            text: "No models available. Check the connection and configuration."
                            color: Theme.textPlaceholder
                            font.pointSize: Theme.smallerFontSize
                        }
                    }

                    // Model rows
                    Repeater {
                        model: detailFlick.providerModels

                        delegate: Column {
                            id: modelDelegate
                            Layout.fillWidth: true

                            required property var modelData
                            required property int index

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: Theme.divider
                                visible: modelDelegate.index > 0
                            }

                            Rectangle {
                                width: parent.width
                                height: 44
                                color: modelRowHover.hovered ? Theme.listItemHoverBg : "transparent"

                                HoverHandler {
                                    id: modelRowHover
                                }

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 16
                                    anchors.rightMargin: 16
                                    spacing: 10

                                    ViciImage {
                                        source: modelDelegate.modelData.icon ?? Img.builtin("computer-chip").withFillColor(Theme.textMuted)
                                        Layout.preferredWidth: 18
                                        Layout.preferredHeight: 18
                                        opacity: modelDelegate.modelData.enabled ? 1.0 : 0.4
                                    }

                                    Text {
                                        text: modelDelegate.modelData.name
                                        color: modelDelegate.modelData.enabled ? Theme.foreground : Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                    }

                                    // Capability badges
                                    Row {
                                        spacing: 4

                                        Repeater {
                                            model: modelDelegate.modelData.capabilities

                                            delegate: Rectangle {
                                                required property string modelData
                                                width: capText.implicitWidth + 10
                                                height: capText.implicitHeight + 4
                                                radius: 4
                                                color: Theme.divider

                                                Text {
                                                    id: capText
                                                    anchors.centerIn: parent
                                                    text: modelData
                                                    color: Theme.textMuted
                                                    font.pointSize: Theme.smallerFontSize - 1
                                                }
                                            }
                                        }
                                    }

                                    Item {
                                        Layout.preferredWidth: 36
                                        Layout.preferredHeight: 20

                                        SettingsToggle {
                                            anchors.right: parent.right
                                            checked: modelDelegate.modelData.enabled
                                            onToggled: root.model.setModelEnabled(root.selectedRow, modelDelegate.modelData.id, checked)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Item {
                implicitHeight: 32
            }

            // ── Danger zone ──
            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: detailCol.sideMargin + 20
                Layout.rightMargin: detailCol.sideMargin + 20
                implicitHeight: dangerCol.implicitHeight + 32
                radius: 8
                color: "transparent"
                border.color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.3)
                border.width: 1

                ColumnLayout {
                    id: dangerCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                    spacing: 12

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Text {
                                text: "Remove provider"
                                color: Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                                font.bold: true
                            }

                            Text {
                                text: "This will remove the provider and all its associated model settings."
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                            }
                        }

                        Rectangle {
                            id: removeBtn
                            implicitWidth: removeBtnText.implicitWidth + 20
                            implicitHeight: 30
                            radius: 6
                            color: removeBtnHover.hovered ? Theme.danger : "transparent"
                            border.color: removeBtnHover.hovered ? Theme.danger : Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.5)
                            border.width: 1

                            Text {
                                id: removeBtnText
                                anchors.centerIn: parent
                                text: "Remove"
                                color: removeBtnHover.hovered ? "#ffffff" : Theme.danger
                                font.pointSize: Theme.smallerFontSize
                            }

                            HoverHandler {
                                id: removeBtnHover
                            }
                            TapHandler {
                                onTapped: {
                                    const row = root.selectedRow;
                                    root.selectedRow = -1;
                                    root.model.removeProvider(row);
                                }
                            }
                        }
                    }
                }
            }

            Item {
                implicitHeight: 24
            }
        }
    }

    // ── Add provider modal ──
    ViciModal {
        id: addModal
        parent: root.Window.contentItem
        width: Math.min(parent.width - 40, 480)
        padding: 24

        property string _providerType: ""
        property string _providerLabel: ""
        property string _providerIcon: ""

        function _reset() {
            modalIdField.text = _providerType === "ollama" ? root.model.nextProviderId(_providerType) : "";
            modalUrlField.text = "http://localhost:11434";
            modalApiKeyField.text = "";
            modalApiKeyField.revealed = false;
        }

        function _submit() {
            const fields = {};
            if (_providerType === "ollama") {
                fields.url = modalUrlField.text.trim();
                fields.id = modalIdField.text.trim();
            } else if (_providerType === "mistral") {
                fields.apiKey = modalApiKeyField.text.trim();
            }
            root.model.addProvider(_providerType, fields);
            close();
        }

        onOpened: {
            if (_providerType === "ollama")
                modalIdField.forceActiveFocus();
            else if (_providerType === "mistral")
                modalApiKeyField.forceActiveFocus();
        }

        contentItem: ColumnLayout {
            spacing: 20

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                ViciImage {
                    source: addModal._providerIcon
                    Layout.preferredWidth: 36
                    Layout.preferredHeight: 36
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "Add " + addModal._providerLabel
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize + 2
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
            }

            // Ollama fields
            ColumnLayout {
                visible: addModal._providerType === "ollama"
                Layout.fillWidth: true
                spacing: 16

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Text {
                        text: "Instance ID"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                    }

                    Text {
                        text: "A unique identifier for this Ollama instance."
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }

                    FormTextInput {
                        id: modalIdField
                        Layout.fillWidth: true
                        placeholder: "ollama"
                        hasError: text.trim().length > 0 && root.model.isProviderIdTaken(text.trim())
                        onAccepted: addModal._submit()
                    }

                    Text {
                        visible: modalIdField.hasError
                        text: "This instance ID is already in use."
                        color: Theme.danger
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Text {
                        text: "Server URL"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                    }

                    Text {
                        text: "The address of your Ollama instance."
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }

                    FormTextInput {
                        id: modalUrlField
                        Layout.fillWidth: true
                        text: "http://localhost:11434"
                        placeholder: "http://localhost:11434"
                        onAccepted: addModal._submit()
                    }
                }
            }

            // Mistral fields
            ColumnLayout {
                visible: addModal._providerType === "mistral"
                Layout.fillWidth: true
                spacing: 6

                Text {
                    text: "API Key"
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                }

                Text {
                    text: "Your Mistral AI API key. You can find it in your Mistral dashboard."
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                FormSecretInput {
                    id: modalApiKeyField
                    placeholder: "sk-..."
                    onAccepted: addModal._submit()
                }
            }

            // Buttons
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 4
                spacing: 10

                Button {
                    id: cancelBtn
                    Layout.fillWidth: true
                    implicitHeight: 32
                    focusPolicy: Qt.StrongFocus
                    activeFocusOnTab: true

                    background: Rectangle {
                        radius: 6
                        color: cancelBtn.hovered || cancelBtn.activeFocus ? Theme.listItemHoverBg : "transparent"
                        border.color: cancelBtn.activeFocus ? Theme.accent : Theme.divider
                        border.width: 1
                    }

                    contentItem: Text {
                        text: "Cancel"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: addModal.close()
                    Keys.onReturnPressed: addModal.close()
                }

                Button {
                    id: addBtn
                    Layout.fillWidth: true
                    implicitHeight: 32
                    focusPolicy: Qt.StrongFocus
                    activeFocusOnTab: true

                    readonly property bool _canSubmit: {
                        if (addModal._providerType === "ollama") {
                            const id = modalIdField.text.trim();
                            return id.length > 0 && !root.model.isProviderIdTaken(id);
                        }
                        if (addModal._providerType === "mistral")
                            return modalApiKeyField.text.trim().length > 0;
                        return true;
                    }

                    background: Rectangle {
                        radius: 6
                        color: addBtn._canSubmit ? (addBtn.hovered || addBtn.activeFocus ? Qt.darker(Theme.accent, 1.1) : Theme.accent) : Theme.listItemHoverBg
                        border.color: addBtn.activeFocus ? Theme.foreground : "transparent"
                        border.width: addBtn.activeFocus ? 1 : 0
                    }

                    contentItem: Text {
                        text: "Add"
                        color: addBtn._canSubmit ? "#ffffff" : Theme.textMuted
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onClicked: {
                        if (_canSubmit)
                            addModal._submit();
                    }
                    Keys.onReturnPressed: {
                        if (_canSubmit)
                            addModal._submit();
                    }
                }
            }
        }
    }
}
