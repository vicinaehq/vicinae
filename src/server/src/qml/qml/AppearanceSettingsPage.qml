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

    ViciWheelHandler {
        target: root
    }

    ScrollBar.vertical: ViciScrollBar {
        policy: ScrollBar.AsNeeded
    }

    ColumnLayout {
        id: outer
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(root.width - 48, 720)
        spacing: 0

        SettingsSectionLabel {
            text: qsTr("Theme")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                label: qsTr("Theme")
                SearchableDropdown {
                    width: parent.width
                    items: root.model.themeItems
                    currentItem: root.model.currentTheme
                    onActivated: item => root.model.selectTheme(item.id)
                }
            }

            SettingsRow {
                label: qsTr("Font")
                SearchableDropdown {
                    width: parent.width
                    items: root.model.fontItems
                    currentItem: root.model.currentFont
                    onActivated: item => root.model.selectFont(item.id)
                }
            }

            SettingsRow {
                label: qsTr("Font size")
                description: qsTr("The base point size used to compute font sizes. Fractional values are accepted. Recommended range is [10.0;12.0].")
                showSeparator: Platform.supports("iconThemeSelection")
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.fontSize
                    placeholder: qsTr("e.g. 11")
                    onAccepted: root.model.fontSize = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.fontSize = text;
                    }
                }
            }

            SettingsRow {
                visible: Platform.supports("iconThemeSelection")
                label: qsTr("Icon Theme")
                description: qsTr("The icon theme used for system icons (applications, mime types, folder icons...). Does not affect builtin Vicinae icons.")
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
            text: qsTr("Window")
            Layout.topMargin: 24
            Layout.bottomMargin: 10
        }

        SettingsGroup {
            SettingsRow {
                visible: Platform.supports("windowMaterial")
                label: qsTr("Window material")
                description: qsTr("Background material applied to the launcher window. Lower the window opacity to see it.")
                SearchableDropdown {
                    width: parent.width
                    items: root.model.windowMaterialItems
                    currentItem: root.model.currentWindowMaterial
                    onActivated: item => root.model.selectWindowMaterial(item.id)
                }
            }

            SettingsRow {
                label: qsTr("Window opacity")
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.windowOpacity
                    placeholder: qsTr("e.g. 1.0")
                    onAccepted: root.model.windowOpacity = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.windowOpacity = text;
                    }
                }
            }

            SettingsRow {
                label: qsTr("Compact mode")
                description: qsTr("Show only the search bar at root; expand when a query is entered.")
                SettingsToggle {
                    checked: root.model.compactMode
                    onToggled: root.model.compactMode = checked
                }
            }

            SettingsRow {
                visible: Platform.supports("layerShell")
                label: qsTr("Use layer shell")
                description: qsTr("Anchor the launcher as a Wayland layer surface (wlr-layer-shell) instead of a regular window. May require reopening Vicinae to fully apply.")
                SettingsToggle {
                    checked: root.model.layerShellEnabled
                    onToggled: checked => root.model.layerShellEnabled = checked
                }
            }

            SettingsRow {
                visible: Platform.supports("clientSideDecorations")
                label: qsTr("Client-side decorations")
                description: qsTr("Let Vicinae draw its own rounded borders and shadow instead of relying on the windowing system.")
                SettingsToggle {
                    checked: root.model.clientSideDecorations
                    onToggled: checked => root.model.clientSideDecorations = checked
                }
            }

            SettingsRow {
                label: qsTr("Corner rounding")
                description: qsTr("Radius of the launcher window corners, in pixels.")
                visible: Platform.supports("customWindowRounding")
                enabled: !Platform.supports("clientSideDecorations") || root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.rounding
                    placeholder: qsTr("e.g. 10")
                    onAccepted: root.model.rounding = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.rounding = text;
                    }
                }
            }

            SettingsRow {
                visible: Platform.supports("clientSideDecorations")
                label: qsTr("Border width")
                description: qsTr("Thickness of the launcher window border, in pixels.")
                enabled: root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.csdBorderWidth
                    placeholder: qsTr("e.g. 3")
                    onAccepted: root.model.csdBorderWidth = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.csdBorderWidth = text;
                    }
                }
            }

            SettingsRow {
                visible: Platform.supports("clientSideDecorations")
                label: qsTr("Shadow size")
                description: qsTr("Size of the drop shadow cast by the launcher window, in pixels.")
                showSeparator: false
                enabled: root.model.clientSideDecorations
                opacity: enabled ? 1 : 0.4
                FormTextInput {
                    width: parent.width
                    releaseFocusOnAccept: true
                    text: root.model.csdShadowSize
                    placeholder: qsTr("e.g. 12")
                    onAccepted: root.model.csdShadowSize = text
                    onEditingChanged: {
                        if (!editing)
                            root.model.csdShadowSize = text;
                    }
                }
            }

            SettingsRow {
                label: qsTr("Native font rendering")
                description: qsTr("Use the platform's native text rendering for system-consistent text. Disable for Qt distance-field rendering (usually faster). May require reopening Vicinae to fully apply.")
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
