import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitHeight: compact ? 28 : 36
    Layout.fillWidth: !compact
    activeFocusOnTab: !compact && !readOnly

    property var items: []
    property var currentItem: null
    signal activated(var item)
    signal popupClosed

    property bool compact: false
    property real minimumWidth: 0

    property string placeholder: ""
    property bool readOnly: false
    property bool hasError: false
    property bool filled: false

    width: compact ? Math.max(triggerButton.implicitWidth, minimumWidth) : implicitWidth

    property real _closedTime: 0

    function open() {
        if (root.readOnly || completionPopup.visible)
            return;
        if (Date.now() - root._closedTime < 300)
            return;
        completionPopup.open();
    }

    function popupX() {
        return Math.min(0, triggerButton.width - completionPopup.width);
    }

    Keys.onReturnPressed: {
        if (!completionPopup.visible)
            open();
    }
    Keys.onSpacePressed: {
        if (!completionPopup.visible)
            open();
    }

    FormInputBackground {
        anchors.fill: triggerButton
        radius: triggerButton.radius
        filled: root.filled || root.compact
        opacity: root.readOnly ? 0.5 : 1.0
    }

    Rectangle {
        id: triggerButton
        opacity: root.readOnly ? 0.5 : 1.0
        implicitWidth: buttonRow.implicitWidth + 20
        anchors.fill: compact ? null : parent
        width: compact ? root.width : implicitWidth
        height: compact ? 28 : implicitHeight
        radius: compact ? 6 : 8
        color: "transparent"
        border.color: Config.withAlpha(root.hasError ? Theme.inputBorderError : (root.activeFocus || completionPopup.visible ? Theme.inputBorderFocus : (compact ? Theme.divider : Theme.inputBorder)), Config.surfaceOpacity)
        border.width: 1

        RowLayout {
            id: buttonRow
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 6

            ViciImage {
                visible: root.currentItem && root.currentItem.iconSource ? true : false
                source: visible ? root.currentItem.iconSource : ""
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                safetyMargins: true
            }

            Text {
                text: root.currentItem?.displayName ?? root.placeholder ?? ""
                color: !compact && !root.currentItem ? Theme.textPlaceholder : Theme.foreground
                font.pointSize: compact ? Theme.smallerFontSize : Theme.regularFontSize
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            ViciImage {
                source: completionPopup.visible ? Img.builtin("chevron-up") : Img.builtin("chevron-down")
                opacity: completionPopup.visible || (buttonMouseArea.containsMouse && !root.readOnly) ? 1.0 : 0.5
                Layout.preferredWidth: 10
                Layout.preferredHeight: 10
            }
        }

        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: root.readOnly ? Qt.ArrowCursor : Qt.PointingHandCursor
            onClicked: root.open()
        }
    }

    CompletionPopup {
        id: completionPopup
        parent: triggerButton
        popupType: Platform.preferItemPopup("dropdown") ? Popup.Item : Popup.Window
        // On Wayland the compositor places the native popup window from the
        // PopupPlacement anchor; x/y only apply on other platforms.
        PopupPlacement.alignment: root.compact ? Qt.AlignRight : Qt.AlignLeft
        x: root.popupX()
        y: triggerButton.height + 4
        width: Math.max(compact ? 200 : 250, root.width)
        focus: true
        sections: root.items
        showFilter: true
        currentItemId: root.currentItem ? root.currentItem.id : ""

        background: Rectangle {
            readonly property bool csd: completionPopup.popupType === Popup.Item || Platform.supports("clientSideDecorations")
            readonly property real bgOpacity: completionPopup.popupType === Popup.Window ? Config.popupOpacity : 1
            radius: csd ? Math.min(Config.borderRounding, 15) : 0
            color: Qt.rgba(Theme.popoverBackground.r, Theme.popoverBackground.g, Theme.popoverBackground.b, bgOpacity)
            border.color: Config.withAlpha(Theme.popoverBorder, bgOpacity)
            border.width: csd ? 1 : 0
            PopupMaterial {}
        }

        onClosed: {
            root._closedTime = Date.now();
            root.popupClosed();
        }

        onItemAccepted: itemData => {
            root.activated(itemData);
        }
    }
}
