import QtQuick
import QtQuick.Layouts

Window {
    id: root

    property int step: 0
    readonly property int stepCount: 4
    readonly property bool onPermissionStep: root.step === 1
    readonly property bool accessibilityGranted: Permissions.accessibilityGranted

    function advance() {
        if (root.step === root.stepCount - 1) {
            onboarding.finish();
            return;
        }
        root.step += 1;
    }

    function goBack() {
        if (root.step > 0)
            root.step -= 1;
    }

    component PermissionRow: SettingsRow {
        id: permissionRow

        property bool granted: false
        signal grant

        controlWidth: 140

        ViciButton {
            visible: !permissionRow.granted
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "Grant Access"
            variant: "accent"
            onClicked: permissionRow.grant()
        }

        RowLayout {
            visible: permissionRow.granted
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 6

            ViciImage {
                source: Img.builtin("check-circle").withFillColor(Theme.toastSuccess)
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
            }

            Text {
                text: "Granted"
                color: Theme.toastSuccess
                font.pointSize: Theme.regularFontSize
            }
        }
    }

    width: 700
    height: 480
    minimumWidth: 700
    minimumHeight: 480
    maximumWidth: 700
    maximumHeight: 480
    visible: true
    color: "transparent"
    flags: Qt.Window
    title: "Welcome to Vicinae"

    WindowMaterial.enabled: Config.blurEnabled
    WindowMaterial.radius: 10

    Binding {
        target: Permissions
        property: "watching"
        value: root.visible && root.onPermissionStep
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, Config.windowOpacity)
        clip: true
        focus: true

        Keys.onReturnPressed: root.advance()
        Keys.onEscapePressed: root.close()

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            StackLayout {
                currentIndex: root.step
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        width: 440
                        spacing: 8

                        ViciImage {
                            source: Img.builtin("vicinae")
                            Layout.preferredWidth: 72
                            Layout.preferredHeight: 72
                            Layout.alignment: Qt.AlignHCenter
                            Layout.bottomMargin: 12
                        }

                        Text {
                            text: "Welcome to Vicinae"
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize + 6
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Let's set it up. It only takes a minute."
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }
                    }
                }

                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        width: 480
                        spacing: 8

                        Text {
                            text: "Permissions"
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize + 6
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Vicinae needs additional permissions in order to make the best of your Mac."
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                            Layout.bottomMargin: 16
                        }

                        SettingsGroup {
                            PermissionRow {
                                label: "Accessibility"
                                description: "Used to paste, expand snippets, and move windows."
                                iconSource: Img.system("accessibility").withFillColor(Theme.foreground)
                                granted: root.accessibilityGranted
                                onGrant: Permissions.requestAccessibility()
                            }

                            PermissionRow {
                                label: "Full Disk Access"
                                description: "Lets search cover your entire disk."
                                iconSource: Img.system("internaldrive").withFillColor(Theme.foreground)
                                showSeparator: false
                                granted: Permissions.fullDiskAccessGranted
                                onGrant: Permissions.requestFullDiskAccess()
                            }
                        }

                        Text {
                            visible: !root.accessibilityGranted || !Permissions.fullDiskAccessGranted
                            text: "You may skip this step; macOS will prompt you for permission later."
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                            Layout.topMargin: 8
                        }
                    }
                }

                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        width: 480
                        spacing: 8

                        Text {
                            text: "Make it your own"
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize + 6
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "You will be able to change these settings later."
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                            Layout.bottomMargin: 16
                        }

                        SettingsGroup {
                            SettingsRow {
                                label: "Theme"
                                description: "Shared across the entire app."

                                SearchableDropdown {
                                    width: parent.width
                                    items: onboarding.generalModel.themeItems
                                    currentItem: onboarding.generalModel.currentTheme
                                    onActivated: item => onboarding.generalModel.selectTheme(item.id)
                                }
                            }

                            SettingsRow {
                                label: "Global hotkey"
                                description: "Opens the launcher from anywhere."
                                showSeparator: onboarding.loginItemSupported

                                ShortcutField {
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    bordered: false
                                    clearable: false
                                    shortcutId: GlobalShortcuts.toggleId
                                    shortcut: onboarding.generalModel.toggleShortcut
                                    onAccepted: shortcut => onboarding.generalModel.toggleShortcut = shortcut
                                }
                            }

                            SettingsRow {
                                visible: onboarding.loginItemSupported
                                label: "Launch at login"
                                description: "Starts Vicinae in the background at login."
                                showSeparator: false

                                SettingsToggle {
                                    checked: onboarding.loginItemEnabled
                                    onToggled: checked => onboarding.loginItemEnabled = checked
                                }
                            }
                        }
                    }
                }

                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        width: 440
                        spacing: 8

                        Text {
                            text: "Setup complete"
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize + 6
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Vicinae is running. Open the launcher with:"
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }

                        ShortcutBadge {
                            visible: onboarding.generalModel.toggleShortcut !== ""
                            tokens: Keyboard.tokensForString(onboarding.generalModel.toggleShortcut)
                            Layout.alignment: Qt.AlignHCenter
                            Layout.topMargin: 8
                            Layout.bottomMargin: 16
                        }

                        Text {
                            text: "Vicinae is open source software."
                            color: Theme.textMuted
                            font.pointSize: Theme.smallerFontSize
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                            Layout.topMargin: 16
                        }

                        RowLayout {
                            spacing: 8
                            Layout.alignment: Qt.AlignHCenter

                            ViciButton {
                                text: "GitHub"
                                variant: "secondary"
                                onClicked: onboarding.openUrl("https://github.com/vicinaehq/vicinae")
                            }

                            ViciButton {
                                text: "Sponsor"
                                variant: "secondary"
                                onClicked: onboarding.openUrl("https://github.com/sponsors/vicinaehq")
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.margins: 16
                implicitHeight: nextButton.implicitHeight

                ViciButton {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Back"
                    visible: root.step > 0
                    onClicked: root.goBack()
                }

                Row {
                    anchors.centerIn: parent
                    spacing: 7

                    Repeater {
                        model: root.stepCount

                        Rectangle {
                            required property int index
                            width: 7
                            height: 7
                            radius: 3.5
                            color: index === root.step ? Theme.accent : Config.withAlpha(Theme.foreground, dotArea.containsMouse ? 0.4 : 0.2)

                            MouseArea {
                                id: dotArea
                                anchors.fill: parent
                                anchors.margins: -5
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.step = index
                            }
                        }
                    }
                }

                ViciButton {
                    id: nextButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                        if (root.step === root.stepCount - 1)
                            return "Finish";
                        if (root.onPermissionStep && !root.accessibilityGranted)
                            return "Set up later";
                        return "Continue";
                    }
                    variant: root.onPermissionStep && !root.accessibilityGranted ? "secondary" : "accent"
                    onClicked: root.advance()
                }
            }
        }
    }
}
