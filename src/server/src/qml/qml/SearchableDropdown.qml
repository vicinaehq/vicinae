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

    width: compact ? Math.max(triggerButton.implicitWidth, minimumWidth) : implicitWidth

    property real _closedTime: 0

    function open() {
        if (root.readOnly || completionPopup.visible)
            return;
        if (Date.now() - root._closedTime < 300)
            return;
        completionPopup.open();
    }

    Keys.onReturnPressed: {
        if (!completionPopup.visible)
            open();
    }
    Keys.onSpacePressed: {
        if (!completionPopup.visible)
            open();
    }

    Rectangle {
        id: triggerButton
        opacity: root.readOnly ? 0.5 : 1.0
        implicitWidth: buttonRow.implicitWidth + 20
        anchors.fill: compact ? null : parent
        width: compact ? root.width : implicitWidth
        height: compact ? 28 : implicitHeight
        radius: compact ? 6 : 8
        color: buttonMouseArea.containsMouse ? Theme.listItemHoverBg : Qt.rgba(Theme.secondaryBackground.r, Theme.secondaryBackground.g, Theme.secondaryBackground.b, Theme.surfaceOpacity)
        border.color: root.hasError ? Theme.inputBorderError : (root.activeFocus || completionPopup.visible ? Theme.inputBorderFocus : (compact ? Theme.divider : Theme.inputBorder))
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
            }

            Text {
                text: root.currentItem ? root.currentItem.displayName : root.placeholder
                color: !compact && !root.currentItem ? Theme.textPlaceholder : Theme.foreground
                font.pointSize: compact ? Theme.smallerFontSize : Theme.regularFontSize
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            ViciImage {
                source: completionPopup.visible ? Img.builtin("chevron-up") : Img.builtin("chevron-down")
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
        popupType: Popup.Window
        x: compact ? triggerButton.width - width : 0
        y: triggerButton.height + 4
        width: Math.max(compact ? 200 : 250, root.width)
        focus: true
        sections: root.items
        showFilter: true
        currentItemId: root.currentItem ? root.currentItem.id : ""

        onClosed: {
            root._closedTime = Date.now();
            root.popupClosed();
        }

        onItemAccepted: itemData => {
            root.activated(itemData);
        }
    }
}
