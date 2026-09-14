pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

Flickable {
    id: root

    required property AISettingsModel model

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

        SettingsSectionLabel {
            text: qsTr("Providers")
            Layout.leftMargin: column.sideMargin
            Layout.topMargin: 24
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

        GridLayout {
            Layout.fillWidth: true
            Layout.leftMargin: column.sideMargin
            Layout.rightMargin: column.sideMargin
            columns: column.columns
            columnSpacing: 12
            rowSpacing: 12

            Repeater {
                model: root.model.providerTypes

                delegate: AIProviderCard {
                    id: card
                    onOpenProvider: id => root.openProvider(id)
                    onSetUp: root.setUp(card.type, card.label, card.icon, card.allowMultiple)
                }
            }
        }

        Item {
            Layout.preferredHeight: 24
        }
    }
}
