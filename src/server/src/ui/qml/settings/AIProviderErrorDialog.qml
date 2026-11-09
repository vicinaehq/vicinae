import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciModal {
    id: root

    required property AIProviderPage page

    parent: Overlay.overlay
    width: Math.min(parent.width - 40, 560)
    padding: 24

    contentItem: ColumnLayout {
        spacing: 16

        Text {
            text: root.page.error
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 1
            font.bold: true
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: Math.min(detailsText.implicitHeight + 20, 320)
            radius: 6
            color: Config.withAlpha(Theme.foreground, 0.05)

            Flickable {
                id: detailsFlick
                anchors.fill: parent
                anchors.margins: 10
                contentHeight: detailsText.implicitHeight
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                ScrollBar.vertical: ScrollBar {}

                TextEdit {
                    id: detailsText
                    width: detailsFlick.width
                    text: root.page.errorDetails
                    color: Theme.textMuted
                    font.family: Theme.monoFontFamily
                    font.pointSize: Theme.smallerFontSize
                    wrapMode: TextEdit.WrapAnywhere
                    readOnly: true
                    selectByMouse: true
                    selectionColor: Theme.textSelectionBg
                    selectedTextColor: Theme.textSelectionFg
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            ViciButton {
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                text: copyTimer.running ? qsTr("Copied!") : qsTr("Copy")
                activeFocusOnTab: true
                onClicked: {
                    root.page.copyErrorDetails();
                    copyTimer.restart();
                }

                Timer {
                    id: copyTimer
                    interval: 1500
                }
            }

            ViciButton {
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                implicitHeight: 32
                variant: "ghost"
                bordered: true
                text: qsTr("Close")
                focus: true
                activeFocusOnTab: true
                onClicked: root.close()
            }
        }
    }
}
