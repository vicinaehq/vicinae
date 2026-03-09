import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    readonly property var model: settings.aiModel

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

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: width
        contentHeight: col.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ViciScrollBar {
            policy: flick.contentHeight > flick.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }

        ColumnLayout {
            id: col
            width: flick.width
            spacing: 0

            readonly property real contentWidth: Math.min(width, 680)
            readonly property real sideMargin: (width - contentWidth) / 2

            Item {
                implicitHeight: 16
            }

            // Header
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: col.sideMargin + 20
                Layout.rightMargin: col.sideMargin + 20
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
                Layout.leftMargin: col.sideMargin + 20
                Layout.rightMargin: col.sideMargin + 20
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

            // Provider list
            Repeater {
                model: root.model

                delegate: Column {
                    id: providerDelegate
                    Layout.fillWidth: true

                    required property int index
                    required property string providerId
                    required property string type
                    required property string typeLabel
                    required property string icon
                    required property string description
                    required property bool expanded
                    required property string url
                    required property string apiKey

                    // Provider row
                    Rectangle {
                        width: parent.width
                        height: 44
                        color: providerHover.hovered ? Theme.listItemHoverBg : "transparent"

                        HoverHandler {
                            id: providerHover
                        }
                        TapHandler {
                            onTapped: root.model.toggleExpanded(providerDelegate.index)
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: col.sideMargin + 20
                            anchors.rightMargin: col.sideMargin + 20
                            spacing: 10

                            ViciImage {
                                source: Img.builtin(providerDelegate.expanded ? "chevron-down-small" : "chevron-right-small").withFillColor(Theme.textMuted)
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                            }

                            ViciImage {
                                source: Img.builtin(providerDelegate.icon).withFillColor(Theme.textMuted)
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                            }

                            Text {
                                text: providerDelegate.providerId
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Text {
                                text: providerDelegate.typeLabel
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }
                    }

                    // Expanded detail
                    Loader {
                        active: providerDelegate.expanded
                        visible: active
                        width: parent.width

                        sourceComponent: Rectangle {
                            implicitHeight: detailCol.implicitHeight + 24
                            color: "transparent"

                            Rectangle {
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: col.sideMargin + 20
                                anchors.rightMargin: col.sideMargin + 20
                                height: 1
                                color: Theme.divider
                            }

                            ColumnLayout {
                                id: detailCol
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.leftMargin: col.sideMargin + 46
                                anchors.rightMargin: col.sideMargin + 20
                                anchors.topMargin: 16
                                spacing: 16

                                // Description
                                Text {
                                    visible: providerDelegate.description !== ""
                                    text: providerDelegate.description
                                    color: Theme.textMuted
                                    font.pointSize: Theme.smallerFontSize
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                }

                                // Ollama config
                                ColumnLayout {
                                    visible: providerDelegate.type === "ollama"
                                    Layout.fillWidth: true
                                    spacing: 4

                                    Text {
                                        text: "Server URL"
                                        color: Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                    }

                                    FormTextInput {
                                        Layout.fillWidth: true
                                        text: providerDelegate.url
                                        placeholder: "http://localhost:11434"
                                        onAccepted: root.model.setField(providerDelegate.index, "url", text.trim())
                                        onEditingChanged: {
                                            if (!editing)
                                                root.model.setField(providerDelegate.index, "url", text.trim());
                                        }
                                    }
                                }

                                // Mistral config
                                ColumnLayout {
                                    visible: providerDelegate.type === "mistral"
                                    Layout.fillWidth: true
                                    spacing: 4

                                    Text {
                                        text: "API Key"
                                        color: Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                    }

                                    RowLayout {
                                        id: apiKeyRow
                                        Layout.fillWidth: true
                                        spacing: 4

                                        property bool revealed: false

                                        FormTextInput {
                                            Layout.fillWidth: true
                                            text: providerDelegate.apiKey
                                            placeholder: "sk-..."
                                            echoMode: apiKeyRow.revealed ? TextInput.Normal : TextInput.Password
                                            onAccepted: root.model.setField(providerDelegate.index, "apiKey", text.trim())
                                            onEditingChanged: {
                                                if (!editing)
                                                    root.model.setField(providerDelegate.index, "apiKey", text.trim());
                                            }
                                        }

                                        Rectangle {
                                            Layout.preferredWidth: 36
                                            Layout.preferredHeight: 36
                                            radius: 8
                                            color: "transparent"
                                            border.color: revealHover.hovered ? Theme.inputBorderFocus : Theme.inputBorder
                                            border.width: 1

                                            ViciImage {
                                                anchors.centerIn: parent
                                                source: Img.builtin(apiKeyRow.revealed ? "eye-disabled" : "eye").withFillColor(Theme.textMuted)
                                                width: 16
                                                height: 16
                                            }

                                            HoverHandler {
                                                id: revealHover
                                            }
                                            TapHandler {
                                                onTapped: apiKeyRow.revealed = !apiKeyRow.revealed
                                            }
                                        }
                                    }
                                }

                                // Models section
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.topMargin: 4
                                    spacing: 8

                                    readonly property var providerModels: root.model.modelsForProvider(providerDelegate.index)

                                    Text {
                                        text: "Models"
                                        color: Theme.foreground
                                        font.pointSize: Theme.smallerFontSize
                                        font.bold: true
                                    }

                                    Text {
                                        visible: parent.providerModels.length === 0
                                        text: "No models available. Check the connection and configuration."
                                        color: Theme.textPlaceholder
                                        font.pointSize: Theme.smallerFontSize
                                    }

                                    Repeater {
                                        model: parent.providerModels

                                        delegate: RowLayout {
                                            required property var modelData
                                            Layout.fillWidth: true
                                            spacing: 8

                                            Text {
                                                text: modelData.name
                                                color: Theme.foreground
                                                font.pointSize: Theme.smallerFontSize
                                                elide: Text.ElideRight
                                                Layout.fillWidth: true
                                            }

                                            Text {
                                                visible: modelData.capabilities !== ""
                                                text: modelData.capabilities
                                                color: Theme.textPlaceholder
                                                font.pointSize: Theme.smallerFontSize
                                            }
                                        }
                                    }
                                }

                                // Remove button
                                Item {
                                    Layout.fillWidth: true
                                    Layout.topMargin: 8
                                    implicitHeight: removeBtn.implicitHeight

                                    Rectangle {
                                        id: removeBtn
                                        anchors.right: parent.right
                                        implicitWidth: removeBtnRow.implicitWidth + 16
                                        implicitHeight: 28
                                        radius: 6
                                        color: removeBtnHover.hovered ? Theme.danger : "transparent"
                                        border.color: removeBtnHover.hovered ? Theme.danger : Theme.inputBorder
                                        border.width: 1

                                        RowLayout {
                                            id: removeBtnRow
                                            anchors.centerIn: parent
                                            spacing: 4

                                            Text {
                                                text: "\u2715"
                                                color: removeBtnHover.hovered ? "#ffffff" : Theme.textMuted
                                                font.pixelSize: 10
                                            }
                                            Text {
                                                text: "Remove"
                                                color: removeBtnHover.hovered ? "#ffffff" : Theme.foreground
                                                font.pointSize: Theme.smallerFontSize
                                            }
                                        }

                                        HoverHandler {
                                            id: removeBtnHover
                                        }
                                        TapHandler {
                                            onTapped: root.model.removeProvider(providerDelegate.index)
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Row separator
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

    // Add provider modal
    Popup {
        id: addModal
        parent: root.Window.contentItem
        anchors.centerIn: parent
        width: Math.min(parent.width - 40, 480)
        padding: 24
        focus: true
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property string _providerType: ""
        property string _providerLabel: ""
        property string _providerIcon: ""

        function _reset() {
            modalIdField.text = _providerType === "ollama" ? root.model.nextProviderId(_providerType) : "";
            modalUrlField.text = "http://localhost:11434";
            modalApiKeyField.text = "";
            modalApiKeyRow.revealed = false;
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

        onActiveFocusChanged: {
            if (!activeFocus && opened)
                close();
        }

        enter: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 150
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "scale"
                    from: 0.95
                    to: 1
                    duration: 150
                    easing.type: Easing.OutCubic
                }
            }
        }

        exit: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 1
                    to: 0
                    duration: 100
                    easing.type: Easing.InCubic
                }
                NumberAnimation {
                    property: "scale"
                    from: 1
                    to: 0.95
                    duration: 100
                    easing.type: Easing.InCubic
                }
            }
        }

        background: Rectangle {
            radius: 10
            color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.98)
            border.color: Theme.divider
            border.width: 1
        }

        Overlay.modal: Rectangle {
            color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.5)
        }

        contentItem: ColumnLayout {
            spacing: 20

            // Header
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
                        onAccepted: addModal._submit()
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

                RowLayout {
                    id: modalApiKeyRow
                    Layout.fillWidth: true
                    spacing: 4

                    property bool revealed: false

                    FormTextInput {
                        id: modalApiKeyField
                        Layout.fillWidth: true
                        placeholder: "sk-..."
                        echoMode: modalApiKeyRow.revealed ? TextInput.Normal : TextInput.Password
                        onAccepted: addModal._submit()
                    }

                    Rectangle {
                        Layout.preferredWidth: 36
                        Layout.preferredHeight: 36
                        radius: 8
                        color: "transparent"
                        border.color: modalRevealHover.hovered ? Theme.inputBorderFocus : Theme.inputBorder
                        border.width: 1

                        ViciImage {
                            anchors.centerIn: parent
                            source: Img.builtin(modalApiKeyRow.revealed ? "eye-disabled" : "eye").withFillColor(Theme.textMuted)
                            width: 16
                            height: 16
                        }

                        HoverHandler {
                            id: modalRevealHover
                        }
                        TapHandler {
                            onTapped: modalApiKeyRow.revealed = !modalApiKeyRow.revealed
                        }
                    }
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
                        if (addModal._providerType === "ollama")
                            return modalIdField.text.trim().length > 0;
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
