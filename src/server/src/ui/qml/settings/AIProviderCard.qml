pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

// One provider type on the AI overview: its instances, or a way to set it up.
SettingsGroup {
    id: root

    required property string type
    required property string label
    required property string description
    required property var icon
    required property bool builtin
    required property bool allowMultiple
    required property var instances

    readonly property bool configured: instances.length > 0

    signal openProvider(string id)
    signal setUp

    Layout.fillHeight: true

    ColumnLayout {
        Layout.fillWidth: true
        Layout.margins: 16
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            spacing: 10

            ViciImage {
                source: root.builtin ? root.icon : root.icon.withFillColor(Theme.foreground)
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
            }

            Text {
                text: root.label
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            ViciIconButton {
                visible: root.allowMultiple && root.configured
                iconSource: Img.icon(BuiltinIcon.Plus)
                tooltip: qsTr("Add instance")
                onClicked: root.setUp()
            }
        }

        Text {
            text: root.description
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.Wrap
            maximumLineCount: 2
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        Item {
            Layout.fillHeight: true
            Layout.preferredHeight: 4
        }

        ViciButton {
            visible: !root.configured
            Layout.alignment: Qt.AlignRight
            implicitHeight: 26
            horizontalPadding: 10
            variant: "ghost"
            bordered: true
            text: qsTr("Set up")
            onClicked: root.setUp()
        }

        RowLayout {
            visible: root.configured && !root.allowMultiple
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                implicitWidth: 6
                implicitHeight: 6
                radius: 3
                color: Theme.toastSuccess
            }

            Text {
                text: root.configured ? root.instances[0].statusText : ""
                color: Theme.textPlaceholder
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            ViciButton {
                implicitHeight: 26
                horizontalPadding: 10
                variant: "ghost"
                bordered: true
                text: qsTr("Manage")
                onClicked: root.openProvider(root.instances[0].id)
            }
        }

        ColumnLayout {
            visible: root.configured && root.allowMultiple
            Layout.fillWidth: true
            spacing: 2

            Repeater {
                model: root.allowMultiple ? root.instances : []

                delegate: Rectangle {
                    id: instanceRow
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: 28
                    radius: 6
                    color: instanceHover.hovered ? Theme.listItemHoverBg : "transparent"

                    HoverHandler {
                        id: instanceHover
                        cursorShape: Qt.PointingHandCursor
                    }

                    TapHandler {
                        onTapped: root.openProvider(instanceRow.modelData.id)
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 6
                        anchors.rightMargin: 6
                        spacing: 8

                        Rectangle {
                            implicitWidth: 6
                            implicitHeight: 6
                            radius: 3
                            color: Theme.toastSuccess
                        }

                        Text {
                            text: instanceRow.modelData.name
                            color: Theme.foreground
                            font.pointSize: Theme.smallerFontSize
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Text {
                            text: instanceRow.modelData.statusText
                            color: Theme.textPlaceholder
                            font.pointSize: Theme.smallerFontSize
                        }

                        ViciImage {
                            source: Img.icon(BuiltinIcon.ChevronRightSmall).withFillColor(Theme.textMuted)
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                        }
                    }
                }
            }
        }
    }
}
