pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: width
        contentHeight: content.implicitHeight
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        topMargin: Style.contentTopInset
        Component.onCompleted: contentY = -topMargin

        ViciWheelHandler {
            target: flickable
        }

        ColumnLayout {
            id: content
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width - 32, 500)
            spacing: 12

            Item {
                implicitHeight: 24
            }

            ViciImage {
                source: Img.icon(BuiltinIcon.Vicinae)
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
                text: Settings.headline
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            Text {
                text: qsTr("Version %1 - Commit %2\n(%3)").arg(Settings.version).arg(Settings.commitHash).arg(Settings.buildInfo)
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }

            Item {
                implicitHeight: 10
            }

            ViciButton {
                iconSource: Img.icon(BuiltinIcon.Github)
                text: "GitHub"
                variant: "secondary"
                radius: 8
                implicitWidth: 200
                onClicked: Settings.openUrl("https://github.com/vicinaehq/vicinae")
                Layout.alignment: Qt.AlignHCenter
            }

            ViciButton {
                iconSource: Img.icon(BuiltinIcon.Book)
                text: qsTr("Documentation")
                variant: "secondary"
                radius: 8
                implicitWidth: 200
                onClicked: Settings.openUrl("https://docs.vicinae.com")
                Layout.alignment: Qt.AlignHCenter
            }

            ViciButton {
                iconSource: Img.icon(BuiltinIcon.Bug)
                text: qsTr("Report a Bug")
                variant: "secondary"
                radius: 8
                implicitWidth: 200
                onClicked: Settings.reportBug()
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
