import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ViciPopover {
    id: root
    surface: "dialog"
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    width: 400
    contentWidth: availableWidth
    padding: 20
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    property bool _confirmed: false
    property Item _focusedButton: null

    onAboutToShow: {
        _confirmed = false;
        Qt.callLater(cancelBtn.forceActiveFocus);
    }
    onClosed: {
        if (!_confirmed)
            launcher.alertModel.cancel();
    }
    onActiveFocusChanged: {
        if (!activeFocus && opened)
            close();
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

            ViciButton {
                id: cancelBtn
                Layout.fillWidth: true
                implicitHeight: 30
                radius: 4
                variant: "ghost"
                bordered: true
                text: launcher.alertModel.cancelText
                foreground: launcher.alertModel.cancelColor
                focus: true
                activeFocusOnTab: true
                showFocus: root._focusedButton === cancelBtn
                onActiveFocusChanged: if (activeFocus)
                    root._focusedButton = cancelBtn
                onClicked: root.close()
                Keys.onRightPressed: confirmBtn.forceActiveFocus()
                Keys.onPressed: event => {
                    const nav = Keyboard.matchNavigation(event.key, event.modifiers);
                    if (nav === 4) {
                        confirmBtn.forceActiveFocus();
                        event.accepted = true;
                    }
                }
            }

            ViciButton {
                id: confirmBtn
                Layout.fillWidth: true
                implicitHeight: 30
                radius: 4
                variant: "ghost"
                bordered: true
                text: launcher.alertModel.confirmText
                foreground: launcher.alertModel.confirmColor
                activeFocusOnTab: true
                showFocus: root._focusedButton === confirmBtn
                onActiveFocusChanged: if (activeFocus)
                    root._focusedButton = confirmBtn
                onClicked: {
                    root._confirmed = true;
                    launcher.alertModel.confirm();
                    root.close();
                }
                Keys.onLeftPressed: cancelBtn.forceActiveFocus()
                Keys.onPressed: event => {
                    const nav = Keyboard.matchNavigation(event.key, event.modifiers);
                    if (nav === 3) {
                        cancelBtn.forceActiveFocus();
                        event.accepted = true;
                    }
                }
            }
        }
    }
}
