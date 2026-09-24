pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Flickable {
    id: root
    readonly property ToolSettingsModel model: Settings.toolModel
    readonly property real contentPaneWidth: Math.min(width - 32, 720)
    readonly property real sideMargin: (width - contentPaneWidth) / 2

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

    ColumnLayout {
        id: column
        width: root.contentPaneWidth
        x: root.sideMargin
        spacing: 0

        ViciButton {
            Layout.topMargin: 16
            implicitHeight: 28
            horizontalPadding: 6
            text: qsTr("AI")
            iconSource: Img.icon(BuiltinIcon.ChevronLeft).withFillColor(Theme.textMuted)
            foreground: Theme.textMuted
            onClicked: Settings.currentSubpage = ""
        }

        Text {
            Layout.topMargin: 16
            text: qsTr("Tools")
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 3
            font.bold: true
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: 6
            text: qsTr("Choose which tools AI models can use from your extensions.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: visible ? 10 : 0
            visible: !root.model.aiEnabled || !root.model.enabled
            text: !root.model.aiEnabled ? qsTr("The AI extension is disabled. Enable it to use tools.") : qsTr("Tools are off. Your individual tool choices are kept.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
        }

        Repeater {
            model: root.model.groups

            delegate: ColumnLayout {
                id: group
                required property var modelData
                Layout.fillWidth: true
                Layout.topMargin: 18
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    ViciImage {
                        source: group.modelData.iconSource
                        Layout.preferredWidth: 18
                        Layout.preferredHeight: 18
                    }

                    Text {
                        text: group.modelData.name
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        font.bold: true
                    }

                    Text {
                        visible: !group.modelData.isEnabled
                        text: qsTr("Disabled")
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                SettingsGroup {
                    Repeater {
                        model: group.modelData.tools

                        delegate: ToolSettingsRow {
                            id: toolRow
                            required property var modelData
                            required property int index
                            tool: modelData
                            showSeparator: index < group.modelData.tools.length - 1
                            onToggled: checked => root.model.setToolEnabled(toolRow.tool.providerId, toolRow.tool.id, checked)
                        }
                    }
                }
            }
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: 28
            horizontalAlignment: Text.AlignHCenter
            visible: root.model.groups.length === 0
            text: qsTr("No tools are available.")
            color: Theme.textMuted
            font.pointSize: Theme.regularFontSize
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
