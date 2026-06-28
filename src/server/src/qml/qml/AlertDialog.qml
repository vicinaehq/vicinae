import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    popupType: Popup.Window
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    width: 400
    contentWidth: availableWidth
    padding: 20
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    PopupPlacement.alignment: Qt.AlignCenter

    property bool _confirmed: false
    property Item _focusedButton: null

    readonly property bool _nativeAnim: Qt.platform.os === "osx" && popupMaterial.macImpl !== null

    onAboutToShow: {
        _confirmed = false;
        Qt.callLater(cancelBtn.forceActiveFocus);
        if (popupMaterial.macImpl)
            popupMaterial.macImpl.animateIn();
    }
    onAboutToHide: if (popupMaterial.macImpl)
        popupMaterial.macImpl.animateOut()
    onClosed: {
        if (!_confirmed)
            launcher.alertModel.cancel();
    }
    onActiveFocusChanged: {
        if (!activeFocus && opened)
            close();
    }

    enter: root._nativeAnim ? null : _itemEnter
    exit: root._nativeAnim ? _holdExit : _itemExit

    property Transition _itemEnter: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                property: "scale"
                from: 0.95
                to: 1
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }

    property Transition _itemExit: Transition {
        ParallelAnimation {
            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 100
                easing.type: Easing.InCubic
            }
            NumberAnimation {
                property: "scale"
                from: 1
                to: 0.95
                duration: 100
                easing.type: Easing.InCubic
            }
        }
    }

    property Transition _holdExit: Transition {
        PauseAnimation {
            duration: 110
        }
    }

    background: Rectangle {
        radius: Math.min(Config.borderRounding, 15)
        color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, Config.windowOpacity)
        border.color: Config.withAlpha(Theme.popoverBorder, Config.windowOpacity)
        border.width: 1

        PopupMaterial {
            id: popupMaterial
        }
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
