import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    required property var host

    focus: true
    Keys.onEscapePressed: root.host.abort()

    // Header with back button
    RowLayout {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 0

        Rectangle {
            id: backBtn
            width: 25
            height: 25
            radius: 4
            color: backArea.containsMouse ? Theme.listItemHoverBg : "transparent"

            ViciImage {
                anchors.centerIn: parent
                width: 16
                height: 16
                source: Img.builtin("arrow-left")
            }

            MouseArea {
                id: backArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.host.abort()
            }
        }

        Item { Layout.fillWidth: true }
    }

    Rectangle {
        id: divider
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: Theme.divider
    }

    // Content area below header — centering happens here
    Item {
        anchors.top: divider.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        // Init state — provider info + continue button
        ColumnLayout {
            anchors.centerIn: parent
            width: Math.min(350, parent.width - 80)
            spacing: 20
            visible: !root.host.success

            ViciImage {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignHCenter
                visible: root.host.providerIconSource !== ""
                source: root.host.providerIconSource
            }

            Text {
                Layout.fillWidth: true
                text: root.host.providerName
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                Layout.fillWidth: true
                text: root.host.providerDescription
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                visible: text !== ""
            }

            Button {
                id: continueBtn
                Layout.alignment: Qt.AlignHCenter
                implicitHeight: 34
                leftPadding: 20
                rightPadding: 20
                focus: true

                background: Rectangle {
                    radius: 4
                    color: continueBtn.hovered || continueBtn.activeFocus
                           ? Theme.accent : Theme.listItemSelectionBg
                    border.color: Theme.accent
                    border.width: 1
                }

                contentItem: Text {
                    text: "Continue with " + root.host.providerName
                    color: continueBtn.hovered || continueBtn.activeFocus
                           ? Theme.listItemSelectionFg : Theme.foreground
                    font.pointSize: Theme.regularFontSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.host.openBrowser()
                Keys.onReturnPressed: root.host.openBrowser()
            }
        }

        // Success state
        ColumnLayout {
            anchors.centerIn: parent
            width: Math.min(350, parent.width - 80)
            spacing: 20
            visible: root.host.success

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                ViciImage {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    visible: root.host.providerIconSource !== ""
                    source: root.host.providerIconSource
                }

                ViciImage {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    source: Img.builtin("check-circle").withFillColor(Theme.toastSuccess)
                }
            }

            Text {
                Layout.fillWidth: true
                text: "You're in!"
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 2
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                Layout.fillWidth: true
                text: "Successfully connected to " + root.host.providerName + ".\nBack to command in an instant..."
                color: Theme.textMuted
                font.pointSize: Theme.regularFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }
        }
    }

    Component.onCompleted: continueBtn.forceActiveFocus()
}
