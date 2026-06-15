import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: outer.implicitHeight
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    readonly property var model: settings.generalModel

    ScrollBar.vertical: ViciScrollBar {
        policy: ScrollBar.AsNeeded
    }

    ColumnLayout {
        id: outer
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(root.width - 48, 720)
        spacing: 0

        SettingsSectionLabel {
            text: "Theme"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Theme"
                SearchableDropdown {
                    width: parent.width
                    items: root.model.themeItems
                    currentItem: root.model.currentTheme
                    onActivated: item => root.model.selectTheme(item.id)
                }
            }

            SettingsRow {
                label: "Font"
                SearchableDropdown {
                    width: parent.width
                    items: root.model.fontItems
                    currentItem: root.model.currentFont
                    onActivated: item => root.model.selectFont(item.id)
                }
            }

            SettingsRow {
                label: "Font size"
                description: "The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0]."
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.fontSize
                    placeholder: "e.g. 11"
                    onAccepted: root.model.fontSize = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.fontSize = text;
                    }
                }
            }

            SettingsRow {
                label: "Icon Theme"
                description: "The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons."
                showSeparator: false
                SearchableDropdown {
                    width: parent.width
                    items: root.model.iconThemeItems
                    currentItem: root.model.currentIconTheme
                    onActivated: item => root.model.selectIconTheme(item.id)
                }
            }
        }

        SettingsSectionLabel {
            text: "Window"
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: "Window opacity"
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.windowOpacity
                    placeholder: "e.g. 1.0"
                    onAccepted: root.model.windowOpacity = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.windowOpacity = text;
                    }
                }
            }

            SettingsRow {
                visible: settings.layerShellSupported
                label: "Use layer shell"
                description: "Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply."
                SettingsToggle {
                    checked: root.model.layerShellEnabled
                    onToggled: checked => root.model.layerShellEnabled = checked
                }
            }

            SettingsRow {
                label: "Client-side decorations"
                description: "Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system."
                SettingsToggle {
                    checked: root.model.clientSideDecorations
                    onToggled: checked => root.model.clientSideDecorations = checked
                }
            }

            SettingsRow {
                label: "Corner rounding"
                description: "Radius of the launcher window corners, in pixels."
                enabled: root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.csdRounding
                    placeholder: "e.g. 10"
                    onAccepted: root.model.csdRounding = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.csdRounding = text;
                    }
                }
            }

            SettingsRow {
                label: "Border width"
                description: "Thickness of the launcher window border, in pixels."
                enabled: root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.csdBorderWidth
                    placeholder: "e.g. 3"
                    onAccepted: root.model.csdBorderWidth = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.csdBorderWidth = text;
                    }
                }
            }

            SettingsRow {
                label: "Shadow size"
                description: "Size of the drop shadow cast by the launcher window, in pixels."
                showSeparator: false
                enabled: root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.csdShadowSize
                    placeholder: "e.g. 12"
                    onAccepted: root.model.csdShadowSize = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.csdShadowSize = text;
                    }
                }
            }

            SettingsRow {
                label: "Native font rendering"
                description: "Use platform/fontconfig text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply."
                SettingsToggle {
                    checked: root.model.nativeTextRendering
                    onToggled: checked => root.model.nativeTextRendering = checked
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
