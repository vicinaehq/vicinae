import QtQuick
import QtQuick.Layouts

Item {
    id: root
    required property var host

    focus: true
    Keys.onEscapePressed: root.host.abort()

    RowLayout {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        spacing: 0

        ViciButton {
            id: backBtn
            width: 25
            height: 25
            radius: 4
            icon: "arrow-left"
            variant: "ghost"
            onClicked: root.host.abort()
        }

        Item {
            Layout.fillWidth: true
        }
    }

    Rectangle {
        id: divider
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: Theme.divider
    }

    Item {
        anchors.top: divider.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

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

            ViciButton {
                id: continueBtn
                Layout.alignment: Qt.AlignHCenter
                implicitHeight: 34
                horizontalPadding: 20
                radius: 4
                text: "Continue with " + root.host.providerName
                variant: "accent"
                focus: true
                onClicked: root.host.openBrowser()
            }
        }

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
