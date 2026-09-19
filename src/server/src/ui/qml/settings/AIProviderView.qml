pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

// One provider: details, configuration fields and models.
Flickable {
    id: root

    required property AIProviderPage page

    signal back

    contentWidth: width
    contentHeight: column.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds
    topMargin: Style.contentTopInset
    Component.onCompleted: contentY = -topMargin

    ViciWheelHandler {
        target: root
    }

    ScrollBar.vertical: ViciScrollBar {
        topPadding: Style.contentTopInset
        bottomPadding: 16
        policy: root.contentHeight > root.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
    }

    component FieldHost: Loader {
        Layout.fillWidth: true

        required property int index
        required property string key
        required property string label
        required property string description
        required property string placeholder
        required property string kind
        required property var options
        required property var value
        required property var currentOption

        sourceComponent: {
            switch (kind) {
            case "text":
                return textComp;
            case "secret":
                return secretComp;
            case "toggle":
                return toggleComp;
            case "select":
                return selectComp;
            default:
                return null;
            }
        }
    }

    AIModelDeleteDialog {
        id: deleteDialog
        onConfirmed: modelId => root.page.removeModel(modelId)
    }

    ColumnLayout {
        id: column
        width: root.width
        spacing: 0

        readonly property real contentWidth: Math.min(width - 32, 760)
        readonly property real sideMargin: (width - contentWidth) / 2

        Item {
            implicitHeight: 16
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            spacing: 0

            Rectangle {
                implicitWidth: backRow.implicitWidth + 12
                implicitHeight: 28
                radius: 6
                color: backHover.hovered ? Theme.listItemHoverBg : "transparent"

                RowLayout {
                    id: backRow
                    anchors.centerIn: parent
                    spacing: 4

                    ViciImage {
                        source: Img.icon(BuiltinIcon.ChevronRightSmall).withFillColor(Theme.textMuted)
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                        rotation: 180
                    }

                    Text {
                        text: qsTr("Providers")
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                HoverHandler {
                    id: backHover
                    cursorShape: Qt.PointingHandCursor
                }

                TapHandler {
                    onTapped: root.back()
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Rectangle {
                visible: !root.page.builtin
                implicitWidth: removeRow.implicitWidth + 12
                implicitHeight: 28
                radius: 6
                color: removeHover.hovered ? Config.withAlpha(Theme.danger, 0.15) : "transparent"

                RowLayout {
                    id: removeRow
                    anchors.centerIn: parent
                    spacing: 4

                    ViciImage {
                        source: Img.icon(BuiltinIcon.Trash).withFillColor(removeHover.hovered ? Theme.danger : Theme.textMuted)
                        Layout.preferredWidth: 14
                        Layout.preferredHeight: 14
                    }

                    Text {
                        text: qsTr("Remove")
                        color: removeHover.hovered ? Theme.danger : Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                HoverHandler {
                    id: removeHover
                    cursorShape: Qt.PointingHandCursor
                }

                TapHandler {
                    onTapped: {
                        root.page.remove();
                        root.back();
                    }
                }
            }
        }

        Item {
            implicitHeight: 16
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            spacing: 12

            ViciImage {
                source: root.page.builtin ? root.page.icon : (root.page.icon ?? Img.icon(BuiltinIcon.ComputerChip)).withFillColor(Theme.foreground)
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                Layout.alignment: Qt.AlignTop
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                RowLayout {
                    spacing: 8

                    Text {
                        text: root.page.name
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize + 1
                        font.bold: true
                    }

                    Text {
                        visible: root.page.typeLabel !== root.page.name
                        text: root.page.typeLabel
                        color: Theme.textPlaceholder
                        font.pointSize: Theme.smallerFontSize
                    }
                }

                Text {
                    visible: root.page.description !== ""
                    text: root.page.description
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

        SettingsSectionTitle {
            visible: configGroup.visible
            text: qsTr("Configuration")
            Layout.leftMargin: column.sideMargin
            Layout.bottomMargin: 8
        }

        SettingsGroup {
            id: configGroup
            visible: fieldsRepeater.count > 0
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.bottomMargin: 24

            readonly property real controlWidth: 300

            Repeater {
                id: fieldsRepeater
                model: root.page.fields

                delegate: FieldHost {}
            }

            Component {
                id: textComp
                SettingsRow {
                    id: field
                    readonly property FieldHost host: parent as FieldHost
                    label: field.host.label
                    description: field.host.description
                    controlWidth: configGroup.controlWidth
                    showSeparator: field.host.index < fieldsRepeater.count - 1

                    FormTextInput {
                        width: parent.width
                        releaseFocusOnAccept: true
                        text: String(field.host.value)
                        placeholder: field.host.placeholder
                        onAccepted: root.page.setField(field.host.key, text.trim())
                        onEditingChanged: {
                            if (!editing)
                                root.page.setField(field.host.key, text.trim());
                        }
                    }
                }
            }

            Component {
                id: secretComp
                SettingsRow {
                    id: field
                    readonly property FieldHost host: parent as FieldHost
                    label: field.host.label
                    description: field.host.description
                    controlWidth: configGroup.controlWidth
                    showSeparator: field.host.index < fieldsRepeater.count - 1

                    FormPasswordInput {
                        width: parent.width
                        releaseFocusOnAccept: true
                        text: String(field.host.value)
                        placeholder: field.host.placeholder
                        onAccepted: root.page.setField(field.host.key, text.trim())
                        onEditingChanged: {
                            if (!editing)
                                root.page.setField(field.host.key, text.trim());
                        }
                    }
                }
            }

            Component {
                id: toggleComp
                SettingsRow {
                    id: field
                    readonly property FieldHost host: parent as FieldHost
                    label: field.host.label
                    description: field.host.description
                    controlWidth: configGroup.controlWidth
                    showSeparator: field.host.index < fieldsRepeater.count - 1

                    SettingsToggle {
                        checked: field.host.value === true
                        onToggled: checked => root.page.setField(field.host.key, checked)
                    }
                }
            }

            Component {
                id: selectComp
                SettingsRow {
                    id: field
                    readonly property FieldHost host: parent as FieldHost
                    label: field.host.label
                    description: field.host.description
                    controlWidth: configGroup.controlWidth
                    showSeparator: field.host.index < fieldsRepeater.count - 1

                    SearchableDropdown {
                        width: parent.width
                        items: field.host.options
                        currentItem: field.host.currentOption
                        onActivated: item => root.page.setField(field.host.key, item.id)
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.bottomMargin: 8

            SettingsSectionTitle {
                text: qsTr("Models")
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: root.page.statusText
                color: Theme.textPlaceholder
                font.pointSize: Theme.smallerFontSize
            }
        }

        SettingsGroup {
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            clip: true

            Item {
                visible: modelsRepeater.count === 0
                Layout.fillWidth: true
                Layout.preferredHeight: 60

                Text {
                    anchors.centerIn: parent
                    text: qsTr("No models available. Check the connection and configuration.")
                    color: Theme.textPlaceholder
                    font.pointSize: Theme.smallerFontSize
                }
            }

            Repeater {
                id: modelsRepeater
                model: root.page.models

                delegate: AIModelRow {
                    id: modelRow
                    onDownload: root.page.download(modelRow.modelId)
                    onCancelDownload: root.page.cancelDownload(modelRow.modelId)
                    onDeleteRequested: deleteDialog.openFor(modelRow.modelId, modelRow.name)
                }
            }
        }

        Item {
            implicitHeight: 24
        }
    }
}
