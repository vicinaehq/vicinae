import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 220

    readonly property var _corePages: [
        { id: "general", icon: "cog", label: "General" },
        { id: "shortcuts", icon: "keyboard", label: "Shortcuts" },
        { id: "advanced", icon: "sliders", label: "Advanced" },
        { id: "about", icon: "vicinae", label: "About" }
    ]

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Text {
            text: "Settings"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
            Layout.leftMargin: 16
            Layout.topMargin: 16
            Layout.bottomMargin: 12
        }

        Repeater {
            model: root._corePages

            delegate: Rectangle {
                id: coreItem
                required property var modelData
                Layout.fillWidth: true
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                height: 32
                radius: 6
                color: settings.currentPage === modelData.id ? Theme.listItemSelectionBg
                       : coreHover.hovered ? Theme.listItemHoverBg
                       : "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 8

                    ViciImage {
                        source: Img.builtin(coreItem.modelData.icon).withFillColor(
                            settings.currentPage === coreItem.modelData.id ? Theme.listItemSelectionFg : Theme.textMuted)
                        Layout.preferredWidth: 16
                        Layout.preferredHeight: 16
                    }

                    Text {
                        text: coreItem.modelData.label
                        color: settings.currentPage === coreItem.modelData.id ? Theme.listItemSelectionFg : Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                HoverHandler { id: coreHover }
                TapHandler {
                    onTapped: settings.currentPage = coreItem.modelData.id
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.leftMargin: 16
            Layout.rightMargin: 16
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            height: 1
            color: Theme.divider
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.bottomMargin: 4
            height: 26
            radius: 4
            color: "transparent"
            border.color: extSearchField.activeFocus ? Theme.inputBorderFocus : Theme.inputBorder
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                spacing: 4

                Image {
                    source: "image://vicinae/builtin:magnifying-glass?fg=" + Theme.textMuted
                    sourceSize.width: 12
                    sourceSize.height: 12
                    Layout.preferredWidth: 12
                    Layout.preferredHeight: 12
                }

                TextInput {
                    id: extSearchField
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    verticalAlignment: TextInput.AlignVCenter
                    font.pointSize: Theme.smallerFontSize
                    color: Theme.foreground
                    clip: true

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        text: "Search extensions..."
                        color: Theme.textPlaceholder
                        font: extSearchField.font
                        visible: !extSearchField.text
                    }

                    Keys.onEscapePressed: text = ""
                }
            }
        }

        ListView {
            id: extList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: {
                const filter = extSearchField.text.toLowerCase()
                if (!filter) return settings.sidebarExtensions
                let result = []
                for (let i = 0; i < settings.sidebarExtensions.length; i++) {
                    const ext = settings.sidebarExtensions[i]
                    if (ext.name.toLowerCase().indexOf(filter) !== -1)
                        result.push(ext)
                }
                return result
            }
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ViciScrollBar {
                policy: extList.contentHeight > extList.height
                        ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            delegate: Item {
                id: extItem
                required property var modelData
                required property int index
                width: extList.width
                height: 32

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    radius: 6
                    color: settings.currentPage === extItem.modelData.providerId ? Theme.listItemSelectionBg
                           : extHover.hovered ? Theme.listItemHoverBg
                           : "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 8

                        ViciImage {
                            source: extItem.modelData.iconSource
                            Layout.preferredWidth: 16
                            Layout.preferredHeight: 16
                        }

                        Text {
                            text: extItem.modelData.name
                            color: settings.currentPage === extItem.modelData.providerId
                                   ? Theme.listItemSelectionFg : Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }

                    HoverHandler { id: extHover }
                    TapHandler {
                        onTapped: settings.currentPage = extItem.modelData.providerId
                    }
                }
            }
        }
    }
}
