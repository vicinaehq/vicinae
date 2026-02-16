import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    readonly property var platformIcons: ({
        "linux": "linux",
        "macOS": "apple",
        "macOS ": "apple",
        "Windows": "windows11",
        "windows": "windows11"
    })

    component TextLink : RowLayout {
        property string label: ""
        property string url: ""

        spacing: 4

        Text {
            text: parent.label
            color: _linkArea.containsMouse ? Theme.accent : Theme.foreground
            font.pointSize: Theme.regularFontSize
        }

        ViciImage {
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            source: Img.builtin("arrow-up-right").withFillColor(
                _linkArea.containsMouse ? Theme.accent : Theme.textMuted
            )
        }

        MouseArea {
            id: _linkArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.host.openUrl(parent.url)
        }
    }

    component SidebarLabel : Text {
        color: Theme.textMuted
        font.pointSize: Theme.smallerFontSize
    }

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: _content.implicitHeight
        clip: true
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: _content
            width: parent.width
            spacing: 0

            // Header
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 25
                spacing: 20

                // Extension icon
                Image {
                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 64
                    Layout.alignment: Qt.AlignTop
                    source: root.host.iconSource
                    sourceSize.width: 64
                    sourceSize.height: 64
                    asynchronous: true
                    cache: true
                }

                // Title + metadata
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        text: root.host.title
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize + 4
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        spacing: 10

                        // Author
                        RowLayout {
                            spacing: 6

                            Image {
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                                source: root.host.authorAvatar
                                sourceSize.width: 16
                                sourceSize.height: 16
                                asynchronous: true
                                cache: true
                            }

                            Text {
                                text: root.host.authorName
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }

                        // Divider
                        Rectangle {
                            width: 1
                            height: 14
                            color: Theme.divider
                        }

                        // Download count
                        RowLayout {
                            spacing: 4

                            ViciImage {
                                Layout.preferredWidth: 14
                                Layout.preferredHeight: 14
                                source: Img.builtin("arrow-down-circle").withFillColor(Theme.textMuted)
                            }

                            Text {
                                text: root.host.downloadCount
                                color: Theme.textMuted
                                font.pointSize: Theme.smallerFontSize
                            }
                        }

                        // Platform icons
                        Repeater {
                            model: root.host.platforms

                            Row {
                                spacing: 0
                                visible: (root.platformIcons[modelData] || "") !== ""

                                // Show divider before first platform icon
                                Rectangle {
                                    visible: index === 0
                                    width: 1
                                    height: 14
                                    color: Theme.divider
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Item { width: index === 0 ? 10 : 5; height: 1 }

                                ViciImage {
                                    width: 14
                                    height: 14
                                    anchors.verticalCenter: parent.verticalCenter
                                    source: {
                                        var iconName = root.platformIcons[modelData] || ""
                                        if (iconName === "") return null
                                        return Img.builtin(iconName).withFillColor(Theme.textMuted)
                                    }
                                }
                            }
                        }

                        Item { Layout.fillWidth: true }
                    }
                }

                // Installed badge
                Rectangle {
                    visible: root.host.isInstalled
                    Layout.alignment: Qt.AlignTop
                    Layout.preferredHeight: 30
                    implicitWidth: _badgeLayout.implicitWidth + 16
                    radius: 6
                    color: Qt.rgba(Theme.toastSuccess.r, Theme.toastSuccess.g, Theme.toastSuccess.b, 0.15)

                    RowLayout {
                        id: _badgeLayout
                        anchors.centerIn: parent
                        spacing: 6

                        ViciImage {
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            source: Img.builtin("check-circle").withFillColor(Theme.toastSuccess)
                        }

                        Text {
                            text: "Installed"
                            color: Theme.toastSuccess
                            font.pointSize: Theme.smallerFontSize
                            font.bold: true
                        }
                    }
                }
            }

            // Alert banner (below header)
            AlertBanner {
                visible: (root.host.alert.message || "") !== ""
                alertType: root.host.alert.type || "info"
                message: root.host.alert.message || ""
                Layout.fillWidth: true
                Layout.leftMargin: 25
                Layout.rightMargin: 25
                Layout.bottomMargin: 5
            }

            // Divider after header
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Theme.divider
            }

            // Screenshots row (Raycast only)
            Flickable {
                visible: root.host.hasScreenshots
                Layout.fillWidth: true
                Layout.preferredHeight: 160
                Layout.margins: 20
                contentWidth: _screenshotRow.width
                clip: true
                flickableDirection: Flickable.HorizontalFlick
                boundsBehavior: Flickable.StopAtBounds

                Row {
                    id: _screenshotRow
                    spacing: 12

                    Repeater {
                        model: root.host.screenshots

                        Image {
                            width: 240
                            height: 150
                            source: modelData
                            sourceSize.width: 240
                            sourceSize.height: 150
                            asynchronous: true
                            cache: true
                            fillMode: Image.PreserveAspectCrop

                            Rectangle {
                                anchors.fill: parent
                                color: "transparent"
                                border.color: Theme.divider
                                border.width: 1
                                radius: 4
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.host.openScreenshot(index)
                            }
                        }
                    }
                }
            }

            // Divider after screenshots
            Rectangle {
                visible: root.host.hasScreenshots
                Layout.fillWidth: true
                height: 1
                color: Theme.divider
            }

            // Content: 2-column layout (main + sidebar)
            RowLayout {
                Layout.fillWidth: true
                spacing: 0

                // Main content (flex 2)
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: 2
                    Layout.alignment: Qt.AlignTop
                    Layout.margins: 20
                    spacing: 20

                    // Description
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        Text {
                            text: "Description"
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            font.bold: true
                        }

                        Text {
                            text: root.host.description
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }
                    }

                    // Divider
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme.divider
                    }

                    // Commands
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 15

                        Text {
                            text: "Commands"
                            color: Theme.textMuted
                            font.pointSize: Theme.regularFontSize
                        }

                        Repeater {
                            model: root.host.commands

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 10

                                    RowLayout {
                                        spacing: 10

                                        Image {
                                            Layout.preferredWidth: 20
                                            Layout.preferredHeight: 20
                                            source: modelData.iconSource
                                            sourceSize.width: 20
                                            sourceSize.height: 20
                                            asynchronous: true
                                            cache: true
                                        }

                                        Text {
                                            text: modelData.title
                                            color: Theme.foreground
                                            font.pointSize: Theme.regularFontSize
                                        }
                                    }

                                    Text {
                                        text: modelData.description
                                        color: Theme.textMuted
                                        font.pointSize: Theme.smallerFontSize
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                        visible: text !== ""
                                    }
                                }

                                // Divider between commands (not after last)
                                Rectangle {
                                    visible: index < root.host.commands.length - 1
                                    Layout.fillWidth: true
                                    Layout.topMargin: 15
                                    height: 1
                                    color: Theme.divider
                                }
                            }
                        }
                    }
                }

                // Vertical divider
                Rectangle {
                    Layout.fillHeight: true
                    width: 1
                    color: Theme.divider
                }

                // Sidebar (flex 1)
                ColumnLayout {
                    Layout.preferredWidth: 1
                    Layout.alignment: Qt.AlignTop
                    Layout.margins: 15
                    spacing: 15

                    // README link
                    ColumnLayout {
                        visible: (root.host.readmeUrl || "") !== ""
                        spacing: 5

                        SidebarLabel { text: "README" }
                        TextLink {
                            label: "Open README"
                            url: root.host.readmeUrl || ""
                        }
                    }

                    // Last update
                    ColumnLayout {
                        spacing: 5

                        SidebarLabel { text: "Last update" }
                        Text {
                            text: root.host.lastUpdate
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                        }
                    }

                    // Contributors (Raycast only)
                    ColumnLayout {
                        visible: root.host.contributors.length > 0
                        spacing: 10

                        SidebarLabel { text: "Contributors" }

                        Repeater {
                            model: root.host.contributors

                            RowLayout {
                                spacing: 8

                                Image {
                                    Layout.preferredWidth: 16
                                    Layout.preferredHeight: 16
                                    source: modelData.avatar
                                    sourceSize.width: 16
                                    sourceSize.height: 16
                                    asynchronous: true
                                    cache: true
                                }

                                Text {
                                    text: modelData.name
                                    color: Theme.foreground
                                    font.pointSize: Theme.smallerFontSize
                                }
                            }
                        }
                    }

                    // Categories (Vicinae only)
                    ColumnLayout {
                        visible: root.host.categories.length > 0
                        spacing: 5

                        SidebarLabel { text: "Categories" }

                        Repeater {
                            model: root.host.categories

                            Text {
                                text: modelData
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                            }
                        }
                    }

                    // Source Code link
                    ColumnLayout {
                        visible: (root.host.sourceUrl || "") !== ""
                        spacing: 5

                        SidebarLabel { text: "Source Code" }
                        TextLink {
                            label: "View Code"
                            url: root.host.sourceUrl || ""
                        }
                    }
                }
            }
        }
    }
}
