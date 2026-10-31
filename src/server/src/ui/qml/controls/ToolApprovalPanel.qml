pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vicinae

ColumnLayout {
    id: root
    required property var approval
    property real maximumDetailsHeight: 140
    signal accepted(bool always)
    signal declined

    spacing: 8

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        ViciImage {
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            source: root.approval.iconSource || Img.icon(BuiltinIcon.WrenchScrewdriver).withFillColor(Theme.textMuted)
        }

        Text {
            Layout.fillWidth: true
            text: qsTr("Allow %1 to run?").arg(root.approval.name)
            textFormat: Text.PlainText
            elide: Text.ElideRight
            color: Theme.foreground
            font.family: Theme.fontFamily
            font.pointSize: Theme.regularFontSize
            font.bold: true
        }
    }

    Rectangle {
        Layout.fillWidth: true
        implicitHeight: Math.min(root.maximumDetailsHeight, details.implicitHeight + 16)
        radius: 6
        color: Config.withAlpha(Theme.foreground, 0.04)
        border.color: Config.withAlpha(Theme.foreground, 0.1)

        Flickable {
            id: scroll
            anchors.fill: parent
            anchors.margins: 8
            contentWidth: width
            contentHeight: details.implicitHeight
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick
            clip: true

            ViciWheelHandler {
                target: scroll
            }

            ScrollBar.vertical: ViciScrollBar {
                policy: scroll.contentHeight > scroll.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            }

            TextEdit {
                id: details
                width: scroll.width
                text: root.approval.summary || root.approval.arguments
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.WrapAnywhere
                readOnly: true
                selectByMouse: true
                selectionColor: Theme.textSelectionBg
                color: Theme.foreground
                font.family: Theme.monoFontFamily
                font.pointSize: Theme.smallerFontSize
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 6

        Item {
            Layout.fillWidth: true
        }

        ViciButton {
            implicitHeight: 28
            variant: "ghost"
            bordered: true
            text: qsTr("Deny")
            activeFocusOnTab: true
            onClicked: root.declined()
        }

        ViciButton {
            implicitHeight: 28
            variant: "ghost"
            bordered: true
            text: qsTr("Always Allow")
            activeFocusOnTab: true
            onClicked: root.accepted(true)
        }

        ViciButton {
            implicitHeight: 28
            variant: "accent"
            text: qsTr("Allow Once")
            activeFocusOnTab: true
            onClicked: root.accepted(false)
        }
    }
}
