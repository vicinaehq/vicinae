import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ViciModal {
    id: root
    width: 400
    padding: 20

    property bool _confirmed: false

    onAboutToShow: _confirmed = false
    onOpened: Qt.callLater(cancelBtn.forceActiveFocus)
    onClosed: {
        if (!_confirmed)
            launcher.alertModel.cancel();
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
                onClicked: root.close()
                Keys.onRightPressed: confirmBtn.forceActiveFocus()
                Keys.onPressed: event => {
                    const nav = launcher.matchNavigationKey(event.key, event.modifiers);
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
                onClicked: {
                    root._confirmed = true;
                    launcher.alertModel.confirm();
                    root.close();
                }
                Keys.onLeftPressed: cancelBtn.forceActiveFocus()
                Keys.onPressed: event => {
                    const nav = launcher.matchNavigationKey(event.key, event.modifiers);
                    if (nav === 3) {
                        cancelBtn.forceActiveFocus();
                        event.accepted = true;
                    }
                }
            }
        }
    }
}
