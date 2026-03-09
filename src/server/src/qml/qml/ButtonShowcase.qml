import QtQuick
import QtQuick.Layouts

Flickable {
    id: root
    contentWidth: width
    contentHeight: content.implicitHeight + 32
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    ColumnLayout {
        id: content
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(parent.width - 32, 500)
        spacing: 24

        Item {
            implicitHeight: 8
        }

        Text {
            text: "Variants"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                text: "Ghost"
                variant: "ghost"
            }
            ViciButton {
                text: "Primary"
                variant: "primary"
            }
            ViciButton {
                text: "Secondary"
                variant: "secondary"
            }
            ViciButton {
                text: "Accent"
                variant: "accent"
            }
        }

        Text {
            text: "Bordered ghost"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                text: "Cancel"
                variant: "ghost"
                bordered: true
                radius: 4
            }
            ViciButton {
                text: "Confirm"
                variant: "ghost"
                bordered: true
                radius: 4
                foreground: Theme.danger
            }
        }

        Text {
            text: "Icon + text"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                icon: "github"
                text: "GitHub"
                variant: "secondary"
                radius: 8
            }
            ViciButton {
                icon: "arrow-left"
                text: "Back"
                variant: "primary"
            }
            ViciButton {
                icon: "bug"
                text: "Report"
                variant: "accent"
                radius: 4
            }
        }

        Text {
            text: "Icon only"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                icon: "arrow-left"
                variant: "primary"
                implicitWidth: 28
                implicitHeight: 28
                iconSize: 14
            }
            ViciButton {
                icon: "arrow-left"
                variant: "ghost"
                implicitWidth: 25
                implicitHeight: 25
                radius: 4
            }
            ViciButton {
                icon: "xmark"
                variant: "ghost"
                implicitWidth: 20
                implicitHeight: 20
                iconSize: 10
                radius: 4
            }
        }

        Text {
            text: "Custom padding"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                text: "Compact"
                variant: "secondary"
                horizontalPadding: 8
            }
            ViciButton {
                text: "Default"
                variant: "secondary"
            }
            ViciButton {
                text: "Wide"
                variant: "secondary"
                horizontalPadding: 20
            }
        }

        Text {
            text: "Focusable (tab through)"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 2
            font.bold: true
        }

        Flow {
            Layout.fillWidth: true
            spacing: 8

            ViciButton {
                text: "First"
                variant: "ghost"
                bordered: true
                radius: 4
                activeFocusOnTab: true
            }
            ViciButton {
                text: "Second"
                variant: "ghost"
                bordered: true
                radius: 4
                activeFocusOnTab: true
            }
            ViciButton {
                text: "Third"
                variant: "ghost"
                bordered: true
                radius: 4
                activeFocusOnTab: true
            }
        }

        Item {
            implicitHeight: 8
        }
    }
}
