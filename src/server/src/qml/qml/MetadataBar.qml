import QtQuick
import QtQuick.Layouts

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

    Flickable {
        anchors.fill: parent
        contentHeight: column.implicitHeight + 20
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: column
            x: 10
            y: 10
            width: parent.width - 20
            spacing: 10

            Repeater {
                model: root.model

                delegate: Loader {
                    Layout.fillWidth: true
                    property var entry: modelData
                    sourceComponent: {
                        var t = (entry && entry.type) || "label"
                        switch (t) {
                        case "separator": return separatorComponent
                        case "link": return linkComponent
                        case "tags": return tagsComponent
                        default: return labelComponent
                        }
                    }
                }
            }
        }
    }

    Component {
        id: labelComponent
        RowLayout {
            spacing: 10

            Text {
                text: entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item { Layout.fillWidth: true }

            ViciImage {
                visible: (entry.icon || "") !== ""
                source: entry.icon || ""
                Layout.preferredWidth: 14
                Layout.preferredHeight: 14
            }

            Text {
                text: entry.value || ""
                color: entry.valueColor || Theme.foreground
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideMiddle
                Layout.maximumWidth: root.width * 0.65
            }
        }
    }

    Component {
        id: linkComponent
        RowLayout {
            spacing: 10

            Text {
                text: entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
            }

            Item { Layout.fillWidth: true }

            Text {
                text: "<a href=\"" + (entry.url || "") + "\">" + (entry.value || "") + "</a>"
                color: Theme.accent
                linkColor: Theme.accent
                font.pointSize: Theme.smallerFontSize
                textFormat: Text.RichText
                elide: Text.ElideMiddle
                Layout.maximumWidth: root.width * 0.65
                onLinkActivated: function(link) { Qt.openUrlExternally(link) }

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
            spacing: 10

            Text {
                text: entry.label || ""
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                Layout.alignment: Qt.AlignTop
            }

            Item { Layout.fillWidth: true }

            Flow {
                Layout.maximumWidth: root.width * 0.65
                Layout.alignment: Qt.AlignRight
                spacing: 4

                Repeater {
                    model: entry.tags || []

                    delegate: Rectangle {
                        required property var modelData
                        width: tagRow.implicitWidth + 12
                        height: tagRow.implicitHeight + 6
                        radius: 4
                        color: modelData.color ? Qt.rgba(
                            Qt.color(modelData.color).r,
                            Qt.color(modelData.color).g,
                            Qt.color(modelData.color).b,
                            0.2
                        ) : Theme.secondaryBackground

                        RowLayout {
                            id: tagRow
                            anchors.centerIn: parent
                            spacing: 4

                            ViciImage {
                                visible: (modelData.icon || "") !== ""
                                source: modelData.icon || ""
                                Layout.preferredWidth: 12
                                Layout.preferredHeight: 12
                            }

                            Text {
                                text: modelData.text || ""
                                color: modelData.color || Theme.foreground
                                font.pointSize: Theme.smallerFontSize
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: separatorComponent
        Rectangle {
            implicitHeight: 1
            color: Theme.divider
        }
    }
}
