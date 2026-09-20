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
    signal setUp(string type, string label, var icon, bool allowMultiple)

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
        width: root.width
        spacing: 0

        readonly property real contentWidth: Math.min(width - 32, 760)
        readonly property real sideMargin: (width - contentWidth) / 2
        readonly property int columns: Math.max(1, Math.floor(contentWidth / 240))

        component ProviderGrid: GridLayout {
            id: grid
            property alias model: repeater.model
            readonly property int count: repeater.count

            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            columns: column.columns
            columnSpacing: 12
            rowSpacing: 12

            Repeater {
                id: repeater

                delegate: AIProviderCard {
                    id: card
                    onOpenProvider: id => root.openProvider(id)
                    onSetUp: root.setUp(card.type, card.label, card.icon, card.allowMultiple)
                }
            }
        }

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

        SettingsSectionLabel {
            text: qsTr("Your providers")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 16
            Layout.bottomMargin: 4
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

        ProviderGrid {
            model: root.model.configuredTypes
        }

        SettingsSectionLabel {
            visible: availableGrid.count > 0
            text: qsTr("Available providers")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 28
            Layout.bottomMargin: 14
        }

        ProviderGrid {
            id: availableGrid
            visible: count > 0
            model: root.model.availableTypes
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
