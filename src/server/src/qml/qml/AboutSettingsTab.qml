import QtQuick
import QtQuick.Layouts

Item {
    id: root

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: content.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: content
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width - 32, 500)
            spacing: 12

            Item { implicitHeight: 24 }

            ViciImage {
                source: Img.builtin("vicinae")
                Layout.preferredWidth: 64
                Layout.preferredHeight: 64
                Layout.alignment: Qt.AlignHCenter
                sourceSize.width: 64
                sourceSize.height: 64
            }

            Text {
                text: "Vicinae"
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize + 4
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Text {
                text: settings.headline
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            Text {
                text: "Version %1 - Commit %2\n(%3)".arg(settings.version).arg(settings.commitHash).arg(settings.buildInfo)
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            Item { implicitHeight: 10 }

            SettingsLinkButton {
                icon: "github"
                label: "GitHub"
                onClicked: settings.openUrl("https://github.com/vicinaehq/vicinae")
                Layout.fillWidth: true
            }

            SettingsLinkButton {
                icon: "book"
                label: "Documentation"
                onClicked: settings.openUrl("https://docs.vicinae.com")
                Layout.fillWidth: true
            }

            SettingsLinkButton {
                icon: "bug"
                label: "Report a Bug"
                onClicked: settings.openUrl("https://github.com/vicinaehq/vicinae/issues/new")
                Layout.fillWidth: true
            }

            Item { Layout.fillHeight: true }
        }
    }
}
