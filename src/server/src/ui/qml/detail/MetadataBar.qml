pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

/// Displays key-value metadata rows with labels left-aligned and values right-aligned.
/// Each entry in `model` is an object with a `type` field:
///   "label" (default): { label, value, icon?, valueColor? }
///   "link":  { label, value, url }
///   "tags":  { label, tags: [{ text, color?, icon? }] }
///   "separator": full-width divider line
/// Scrolls vertically when content exceeds available height.
Item {
    id: root
    property var model: []
    implicitHeight: column.implicitHeight + 20  // 2 * margins

    StatusBarInset {
        id: statusBarInset
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: column.implicitHeight + 20
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        bottomMargin: statusBarInset.value

        ViciWheelHandler {
            target: flickable
        }

        ColumnLayout {
            id: column
            x: 10
            y: 10
            width: parent.width - 20
            spacing: 10

            Repeater {
                model: root.model

                delegate: EntryHost {}
            }
        }
    }

    component EntryHost: Loader {
                    Layout.fillWidth: true
                    required property var modelData
                    readonly property var entry: modelData
                    sourceComponent: {
                        var t = (entry && entry.type) || "label";
                        switch (t) {
                        case "separator":
                            return separatorComponent;
                        case "link":
                            return linkComponent;
                        case "tags":
                            return tagsComponent;
                        case "icons":
                            return iconsComponent;
                        default:
                            return labelComponent;
                        }
                    }
    }

    Component {
        id: labelComponent
        RowLayout {
            id: row
            readonly property EntryHost host: parent as EntryHost
            spacing: 10

            Text {
                text: row.host.entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item {
                Layout.fillWidth: true
            }

            ViciImage {
                visible: (row.host.entry.icon || "") !== ""
                source: row.host.entry.icon || ""
                Layout.preferredWidth: 14
                Layout.preferredHeight: 14
            }

            Text {
                text: row.host.entry.value || ""
                color: row.host.entry.valueColor || Theme.foreground
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideMiddle
                Layout.maximumWidth: root.width * 0.65
            }
        }
    }

    Component {
        id: linkComponent
        RowLayout {
            id: row
            readonly property EntryHost host: parent as EntryHost
            spacing: 10

            Text {
                text: row.host.entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item {
                Layout.fillWidth: true
            }

            Text {
                text: "<a href=\"" + (row.host.entry.url || "") + "\" style=\"color:" + Theme.linkColor + ";\">" + (row.host.entry.value || "") + "</a>"
                color: Theme.linkColor
                linkColor: Theme.linkColor
                font.pointSize: Theme.smallerFontSize
                textFormat: Text.RichText
                elide: Text.ElideMiddle
                Layout.maximumWidth: root.width * 0.65
                onLinkActivated: function (link) {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    acceptedButtons: Qt.NoButton
                }
            }
        }
    }

    Component {
        id: tagsComponent
        RowLayout {
            id: row
            readonly property EntryHost host: parent as EntryHost
            spacing: 10

            Text {
                text: row.host.entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                Layout.alignment: Qt.AlignTop
            }

            Item {
                Layout.fillWidth: true
            }

            Flow {
                Layout.maximumWidth: root.width * 0.65
                Layout.alignment: Qt.AlignRight
                spacing: 4

                Repeater {
                    model: row.host.entry.tags || []

                    delegate: Rectangle {
                        id: tag
                        required property var modelData
                        width: tagRow.implicitWidth + 12
                        height: tagRow.implicitHeight + 6
                        radius: 4
                        color: modelData.color ? Qt.rgba(Qt.color(modelData.color).r, Qt.color(modelData.color).g, Qt.color(modelData.color).b, 0.2) : Theme.secondaryBackground

                        RowLayout {
                            id: tagRow
                            anchors.centerIn: parent
                            spacing: 4

                            ViciImage {
                                visible: (tag.modelData.icon || "") !== ""
                                source: tag.modelData.icon || ""
                                Layout.preferredWidth: 12
                                Layout.preferredHeight: 12
                            }

                            Text {
                                text: tag.modelData.text || ""
                                color: tag.modelData.color || Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: iconsComponent
        RowLayout {
            id: row
            readonly property EntryHost host: parent as EntryHost
            spacing: 10

            Text {
                text: row.host.entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item {
                Layout.fillWidth: true
            }

            Row {
                id: iconsRow
                spacing: 4
                Layout.alignment: Qt.AlignRight

                readonly property int maxVisible: 6
                readonly property var icons: row.host.entry.icons || []
                readonly property int overflow: Math.max(0, icons.length - maxVisible)

                Repeater {
                    model: iconsRow.icons.slice(0, iconsRow.maxVisible)

                    delegate: Item {
                        id: iconItem
                        required property var modelData
                        width: 16
                        height: 16

                        ViciImage {
                            anchors.fill: parent
                            source: iconItem.modelData.icon || ""
                        }

                        HoverHandler {
                            id: iconHover
                        }

                        ViciToolTip {
                            visible: iconHover.hovered
                            text: iconItem.modelData.tooltip || ""
                        }
                    }
                }

                Text {
                    visible: iconsRow.overflow > 0
                    text: "+" + iconsRow.overflow
                    color: Theme.textMuted
                    font.pointSize: Theme.smallerFontSize
                    verticalAlignment: Text.AlignVCenter
                    height: 16
                }
            }
        }
    }

    Component {
        id: separatorComponent
        ViciDivider {}
    }
}
