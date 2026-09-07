import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Action panel page that records a shortcut in place. Backed by a
// ShortcutRecorderPanelView exposed as `view`.
Item {
    id: root

    required property var view
    property var controller: actionPanel

    // Tells the window-level key routing to deliver every keypress here
    // instead of matching bound actions or app keybinds.
    readonly property bool capturesAllKeys: true

    signal navigateBack

    readonly property int stripeHeight: 40
    readonly property int captureAreaHeight: 130

    implicitHeight: stripeHeight + 1 + captureAreaHeight

    function focusFilter() {
        capture.forceActiveFocus();
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: stripe
            Layout.fillWidth: true
            Layout.preferredHeight: root.stripeHeight

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                ViciImage {
                    source: root.view.icon
                    sourceSize.width: 20
                    sourceSize.height: 20
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignVCenter
                }

                Text {
                    Layout.fillWidth: true
                    text: root.view.title
                    color: Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    elide: Text.ElideRight
                }
            }
        }

        Rectangle {
            id: divider
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.divider
        }

        Item {
            id: captureArea
            Layout.fillWidth: true
            Layout.preferredHeight: root.captureAreaHeight

            ShortcutRecorderCapture {
                id: capture
                anchors.centerIn: parent
                width: parent.width - 32
                focus: true
                capturing: root.StackView.status === StackView.Active
                initialTokens: Keyboard.tokensForString(root.view.currentShortcut)
                validateShortcut: (key, mods) => GlobalShortcuts.validate(key, mods, root.view.shortcutId)
                shortcutDisplayProvider: (key, mods) => Keyboard.tokens(key, mods)

                onShortcutCaptured: (key, modifiers) => {
                    root.view.accept(key, modifiers);
                    root.view.requestClose();
                }
                onDismissRequested: key => {
                    if (key === Qt.Key_Backspace && root.view.currentShortcut !== "") {
                        root.view.clear();
                        root.view.requestClose();
                    } else {
                        root.navigateBack();
                    }
                }
            }

            Text {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                visible: root.view.currentShortcut !== ""
                text: qsTr("Press Backspace to remove the current shortcut")
                color: Theme.textPlaceholder
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
            }
        }
    }
}
