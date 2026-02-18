import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    anchors.centerIn: parent
    width: 400
    padding: 20
    focus: true
    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property bool _confirmed: false

    onAboutToShow: _confirmed = false
    onOpened: Qt.callLater(cancelBtn.forceActiveFocus)
    onClosed: {
        if (!_confirmed)
            launcher.alertModel.cancel()
    }

    background: Rectangle {
        radius: 6
        color: Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, 0.98)
        border.color: Theme.divider
        border.width: 1
    }

    Overlay.modal: Rectangle {
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.5)
    }

    contentItem: ColumnLayout {
        spacing: 15

        ViciImage {
            Layout.alignment: Qt.AlignHCenter
            width: 30
            height: 30
            visible: launcher.alertModel.iconSource !== ""
            source: launcher.alertModel.iconSource
        }

        Text {
            Layout.fillWidth: true
            text: launcher.alertModel.title
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        Text {
            Layout.fillWidth: true
            text: launcher.alertModel.message
            color: Theme.textMuted
            font.pointSize: Theme.regularFontSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: text !== ""
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                id: cancelBtn
                Layout.fillWidth: true
                implicitHeight: 30
                focus: true
                focusPolicy: Qt.StrongFocus
                activeFocusOnTab: true

                background: Rectangle {
                    radius: 4
                    color: cancelBtn.hovered || cancelBtn.activeFocus
                           ? Theme.listItemHoverBg : "transparent"
                    border.color: cancelBtn.activeFocus ? Theme.accent : Theme.divider
                    border.width: 1
                }

                contentItem: Text {
                    text: launcher.alertModel.cancelText
                    color: launcher.alertModel.cancelColor
                    font.pointSize: Theme.regularFontSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.close()
                Keys.onReturnPressed: root.close()
                Keys.onRightPressed: confirmBtn.forceActiveFocus()
            }

            Button {
                id: confirmBtn
                Layout.fillWidth: true
                implicitHeight: 30
                focusPolicy: Qt.StrongFocus
                activeFocusOnTab: true

                background: Rectangle {
                    radius: 4
                    color: confirmBtn.hovered || confirmBtn.activeFocus
                           ? Theme.listItemHoverBg : "transparent"
                    border.color: confirmBtn.activeFocus ? Theme.accent : Theme.divider
                    border.width: 1
                }

                contentItem: Text {
                    text: launcher.alertModel.confirmText
                    color: launcher.alertModel.confirmColor
                    font.pointSize: Theme.regularFontSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    root._confirmed = true
                    launcher.alertModel.confirm()
                    root.close()
                }
                Keys.onReturnPressed: {
                    root._confirmed = true
                    launcher.alertModel.confirm()
                    root.close()
                }
                Keys.onLeftPressed: cancelBtn.forceActiveFocus()
            }
        }
    }
}
