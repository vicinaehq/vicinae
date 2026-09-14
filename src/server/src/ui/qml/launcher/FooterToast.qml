pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

RowLayout {
    spacing: 6

    Rectangle {
        visible: Launcher.toastStyle !== 4
        Layout.preferredWidth: 10
        Layout.preferredHeight: 10
        Layout.alignment: Qt.AlignVCenter
        radius: 5
        color: {
            switch (Launcher.toastStyle) {
            case 0:
                return Theme.toastSuccess;
            case 1:
                return Theme.toastInfo;
            case 2:
                return Theme.toastWarning;
            case 3:
                return Theme.toastDanger;
            default:
                return Theme.toastInfo;
            }
        }
    }

    ViciSpinner {
        visible: Launcher.toastStyle === 4
        color: Config.withAlpha(Theme.textMuted, Config.windowOpacity)
        Layout.preferredWidth: 12
        Layout.preferredHeight: 12
        Layout.alignment: Qt.AlignVCenter
    }

    Text {
        text: Launcher.toastTitle
        color: Theme.foreground
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
    }

    Text {
        text: Launcher.toastMessage
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pointSize: Theme.smallerFontSize
        visible: Launcher.toastMessage !== ""
        maximumLineCount: 1
        elide: Text.ElideRight
        Layout.fillWidth: true
    }

    Item {
        Layout.fillWidth: true
    }
}
