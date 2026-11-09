pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Flickable {
    id: root

    required property AISettingsModel model
    property string backLabel: ""

    signal back
    signal openProvider(string id)
    signal addProvider

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

    component ProviderRow: ColumnLayout {
        id: row
        property var iconSource
        property string title: ""
        property string subtitle: ""
        property string status: ""
        property string health: ""
        property bool showSeparator: true
        signal clicked

        Layout.fillWidth: true
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.margins: 4
            implicitHeight: 48
            radius: 8
            color: rowHover.hovered ? Theme.listItemHoverBg : "transparent"

            HoverHandler {
                id: rowHover
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                onTapped: row.clicked()
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 12

                ViciImage {
                    source: row.iconSource
                    Layout.preferredWidth: 22
                    Layout.preferredHeight: 22
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: row.title
                        color: Theme.foreground
                        font.pointSize: Theme.regularFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Text {
                        visible: row.subtitle !== ""
                        text: row.subtitle
                        color: Theme.textMuted
                        font.pointSize: Theme.smallerFontSize
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                Rectangle {
                    implicitWidth: 6
                    implicitHeight: 6
                    radius: 3
                    color: row.health === "error" ? Theme.toastDanger : row.health === "ready" ? Theme.toastSuccess : Theme.textPlaceholder
                }

                Text {
                    text: row.status
                    color: Theme.textPlaceholder
                    font.pointSize: Theme.smallerFontSize
                    elide: Text.ElideRight
                }

                ViciImage {
                    source: Img.icon(BuiltinIcon.ChevronRightSmall).withFillColor(Theme.textMuted)
                    Layout.preferredWidth: 14
                    Layout.preferredHeight: 14
                }
            }
        }

        ViciDivider {
            visible: row.showSeparator
            Layout.fillWidth: true
            Layout.leftMargin: 16
            Layout.rightMargin: 16
        }
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

        Rectangle {
            Layout.leftMargin: column.sideMargin
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
                    text: root.backLabel
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

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.topMargin: 16
            Layout.bottomMargin: 4

            SettingsSectionLabel {
                text: qsTr("Your providers")
            }

            ViciButton {
                implicitHeight: 26
                horizontalPadding: 10
                variant: "ghost"
                bordered: true
                text: qsTr("Add provider")
                onClicked: root.addProvider()
            }
        }

        Text {
            text: qsTr("Models from every provider you set up are available to all AI features, including dictation.")
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.bottomMargin: 14
        }

        Rectangle {
            id: unencryptedWarning
            visible: !root.model.secretsEncrypted
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            Layout.bottomMargin: 14
            implicitHeight: warningContent.implicitHeight + 20
            radius: 8
            color: Qt.rgba(Theme.toastWarning.r, Theme.toastWarning.g, Theme.toastWarning.b, 0.1)
            border.color: Qt.rgba(Theme.toastWarning.r, Theme.toastWarning.g, Theme.toastWarning.b, 0.3)
            border.width: 1

            RowLayout {
                id: warningContent
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                ViciImage {
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    Layout.alignment: Qt.AlignTop
                    source: Img.icon(BuiltinIcon.Warning).withFillColor(Theme.toastWarning)
                }

                Text {
                    text: qsTr("API keys are stored unencrypted. To protect them, turn on Encrypt sensitive data in the Advanced settings.")
                    color: Theme.foreground
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
            }
        }

        SettingsGroup {
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin

            Text {
                visible: types.count === 0
                text: qsTr("No providers yet. Add one to start using AI features.")
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                Layout.margins: 16
            }

            Repeater {
                id: types
                model: root.model.configuredTypes

                delegate: ColumnLayout {
                    id: typeRow
                    required property int index
                    required property string label
                    required property var icon
                    required property bool builtin
                    required property bool allowMultiple
                    required property var instances
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        model: typeRow.instances

                        delegate: ProviderRow {
                            id: instanceRow
                            required property int index
                            required property var modelData
                            iconSource: typeRow.builtin ? typeRow.icon : typeRow.icon.withFillColor(Theme.foreground)
                            title: typeRow.allowMultiple ? instanceRow.modelData.name : typeRow.label
                            subtitle: typeRow.allowMultiple ? typeRow.label : ""
                            status: instanceRow.modelData.statusText
                            health: instanceRow.modelData.health
                            showSeparator: typeRow.index < types.count - 1 || instanceRow.index < typeRow.instances.length - 1
                            onClicked: root.openProvider(instanceRow.modelData.id)
                        }
                    }
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
