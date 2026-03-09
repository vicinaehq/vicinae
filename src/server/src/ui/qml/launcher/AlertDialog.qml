pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ViciPopover {
    id: root
    surface: "dialog"
    frostedOpacity: 0.7
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
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
            Launcher.alertModel.cancel();
    }
    onActiveFocusChanged: {
        if (!activeFocus && opened)
            close();
    }

    contentItem: ColumnLayout {
        spacing: 15

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            visible: Launcher.alertModel.iconSource !== ""
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            radius: width / 2
            color: Launcher.alertModel.iconBadged ? Config.withAlpha(Launcher.alertModel.confirmColor, 0.14) : "transparent"

            ViciImage {
                anchors.centerIn: parent
                width: Launcher.alertModel.iconBadged ? 22 : 30
                height: width
                source: Launcher.alertModel.iconSource
            }
        }

        Text {
            Layout.fillWidth: true
            text: Launcher.alertModel.title
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }

        Text {
            Layout.fillWidth: true
            text: Launcher.alertModel.message
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
                implicitHeight: 32
                variant: "secondary"
                text: Launcher.alertModel.cancelText
                foreground: Launcher.alertModel.cancelColor
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
                implicitHeight: 32
                variant: "tinted"
                text: Launcher.alertModel.confirmText
                foreground: Launcher.alertModel.confirmColor
                activeFocusOnTab: true
                showFocus: root._focusedButton === confirmBtn
                onActiveFocusChanged: if (activeFocus)
                    root._focusedButton = confirmBtn
                onClicked: {
                    root._confirmed = true;
                    Launcher.alertModel.confirm();
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
