import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    readonly property var extModel: settings.extensionModel
    readonly property string providerId: settings.currentPage

    Component.onCompleted: root.extModel.selectProviderById(root.providerId)
    onProviderIdChanged: root.extModel.selectProviderById(root.providerId)

    Loader {
        anchors.fill: parent
        sourceComponent: {
            switch (settings.currentPage) {
            default: return defaultExtensionPage
            }
        }
    }

    Component {
        id: defaultExtensionPage

        Flickable {
            id: flickable
            contentWidth: width
            contentHeight: pageCol.implicitHeight
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: flickable.contentHeight > flickable.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            ColumnLayout {
                id: pageCol
                width: Math.min(parent.width, 680)
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 0

                Item { implicitHeight: 24 }

                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    spacing: 8

                    ViciImage {
                        source: root.extModel.selectedIconSource
                        Layout.preferredWidth: 48
                        Layout.preferredHeight: 48
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: root.extModel.selectedTitle
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize + 2
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Text {
                        visible: root.extModel.selectedDescription !== ""
                        text: root.extModel.selectedDescription
                        color: Theme.textMuted
                        font.pointSize: Theme.regularFontSize
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }

                Item { implicitHeight: 16 }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    height: 1
                    color: Theme.divider
                }

                // Provider preferences
                ColumnLayout {
                    visible: root.extModel.hasPreferences
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    Layout.topMargin: 16
                    spacing: 0

                    Text {
                        text: "Preferences"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        Layout.bottomMargin: 12
                    }

                    SettingsPreferenceForm {
                        Layout.fillWidth: true
                        prefModel: root.extModel.preferenceModel
                    }

                    Item { implicitHeight: 16 }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme.divider
                    }
                }

                // Metadata
                ColumnLayout {
                    visible: root.extModel.selectedMetadata.length > 0
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    Layout.topMargin: 12
                    spacing: 0

                    Repeater {
                        model: root.extModel.selectedMetadata

                        delegate: ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.topMargin: 5
                                Layout.bottomMargin: 5
                                spacing: 10

                                Text {
                                    text: modelData.key
                                    color: Theme.textMuted
                                    font.pointSize: Theme.smallerFontSize
                                }

                                Item { Layout.fillWidth: true }

                                Text {
                                    text: modelData.value
                                    color: Theme.foreground
                                    font.pointSize: Theme.smallerFontSize
                                    horizontalAlignment: Text.AlignRight
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                height: 1
                                color: Theme.divider
                            }
                        }
                    }

                    Item { implicitHeight: 8 }
                }

                // Commands section
                ColumnLayout {
                    id: commandsSection
                    visible: commandRepeater.count > 0
                    Layout.fillWidth: true
                    Layout.topMargin: 12
                    spacing: 0

                    property string expandedCommandId: ""

                    Text {
                        text: "Commands"
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                        Layout.leftMargin: 20
                        Layout.rightMargin: 20
                        Layout.bottomMargin: 8
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.leftMargin: 20
                        Layout.rightMargin: 20
                        height: 1
                        color: Theme.divider
                    }

                    Repeater {
                        id: commandRepeater
                        model: root.extModel.currentProviderCommands

                        delegate: ColumnLayout {
                            id: cmdDelegate
                            Layout.fillWidth: true
                            spacing: 0

                            required property var modelData
                            required property int index

                            readonly property bool isExpanded: commandsSection.expandedCommandId === modelData.entrypointId

                            Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: cmdRow.implicitHeight + 16
                                color: cmdHover.hovered ? Theme.listItemHoverBg : "transparent"

                                HoverHandler { id: cmdHover }
                                TapHandler {
                                    onTapped: {
                                        if (cmdDelegate.isExpanded) {
                                            commandsSection.expandedCommandId = ""
                                        } else {
                                            commandsSection.expandedCommandId = cmdDelegate.modelData.entrypointId
                                            root.extModel.loadCommandPreferences(cmdDelegate.modelData.entrypointId)
                                        }
                                    }
                                }

                                RowLayout {
                                    id: cmdRow
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.leftMargin: 20
                                    anchors.rightMargin: 20
                                    spacing: 10

                                    ViciImage {
                                        source: Img.builtin(cmdDelegate.isExpanded ? "chevron-down-small" : "chevron-right-small")
                                            .withFillColor(Theme.textMuted)
                                        Layout.preferredWidth: 16
                                        Layout.preferredHeight: 16
                                    }

                                    ViciImage {
                                        source: cmdDelegate.modelData.iconSource
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 1

                                        Text {
                                            text: cmdDelegate.modelData.name
                                            color: !cmdDelegate.modelData.enabled ? Theme.textMuted : Theme.foreground
                                            font.pointSize: Theme.regularFontSize
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            visible: cmdDelegate.modelData.type !== ""
                                            text: cmdDelegate.modelData.type
                                            color: Theme.textMuted
                                            font.pointSize: Theme.smallerFontSize
                                        }
                                    }

                                    Rectangle {
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        radius: 4
                                        color: cmdDelegate.modelData.enabled ? Theme.accent : "transparent"
                                        border.color: cmdDelegate.modelData.enabled ? Theme.accent : Theme.inputBorder
                                        border.width: 1

                                        Text {
                                            anchors.centerIn: parent
                                            text: "\u2713"
                                            color: "#ffffff"
                                            font.pixelSize: 13
                                            font.bold: true
                                            visible: cmdDelegate.modelData.enabled
                                        }

                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: root.extModel.setEnabledByEntrypointId(
                                                cmdDelegate.modelData.entrypointId, !cmdDelegate.modelData.enabled)
                                        }
                                    }
                                }
                            }

                            // Expanded command details
                            ColumnLayout {
                                visible: cmdDelegate.isExpanded
                                Layout.fillWidth: true
                                Layout.leftMargin: 56
                                Layout.rightMargin: 20
                                Layout.bottomMargin: 12
                                spacing: 8

                                Text {
                                    visible: cmdDelegate.modelData.description !== ""
                                    text: cmdDelegate.modelData.description
                                    color: Theme.textMuted
                                    font.pointSize: Theme.smallerFontSize
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Text {
                                        text: "Alias"
                                        color: Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                        Layout.alignment: Qt.AlignVCenter
                                    }

                                    Rectangle {
                                        Layout.preferredWidth: 180
                                        height: 28
                                        radius: 4
                                        color: "transparent"
                                        border.color: cmdAliasInput.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
                                        border.width: 1

                                        TextInput {
                                            id: cmdAliasInput
                                            anchors.fill: parent
                                            anchors.leftMargin: 6
                                            anchors.rightMargin: 6
                                            verticalAlignment: TextInput.AlignVCenter
                                            text: cmdDelegate.modelData.alias
                                            color: Theme.foreground
                                            font.pointSize: Theme.smallerFontSize
                                            clip: true

                                            Text {
                                                anchors.fill: parent
                                                verticalAlignment: Text.AlignVCenter
                                                text: "Add alias"
                                                color: Theme.textPlaceholder
                                                font: parent.font
                                                visible: !parent.text && !parent.activeFocus
                                            }

                                            onActiveFocusChanged: {
                                                if (!activeFocus)
                                                    root.extModel.setAliasByEntrypointId(
                                                        cmdDelegate.modelData.entrypointId, text)
                                            }
                                            onAccepted: root.extModel.setAliasByEntrypointId(
                                                cmdDelegate.modelData.entrypointId, text)
                                        }
                                    }
                                }

                                // Per-command preferences
                                SettingsPreferenceForm {
                                    visible: cmdDelegate.isExpanded && root.extModel.commandPreferenceModel.rowCount() > 0
                                    Layout.fillWidth: true
                                    prefModel: root.extModel.commandPreferenceModel
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.leftMargin: 20
                                Layout.rightMargin: 20
                                height: 1
                                color: Theme.divider
                            }
                        }
                    }
                }

                Item { implicitHeight: 24 }
            }
        }
    }
}
