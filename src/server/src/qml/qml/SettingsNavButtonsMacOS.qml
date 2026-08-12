import QtQuick
import QtQuick.Layouts

// System Settings style back/forward capsule.
Rectangle {
    id: root
    implicitWidth: 68
    implicitHeight: 30
    radius: height / 2
    color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.055)
    border.width: 1
    border.color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.05)

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                anchors.fill: parent
                topLeftRadius: height / 2
                bottomLeftRadius: height / 2
                color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.07)
                visible: backHover.hovered && settings.canGoBack
            }

            ViciImage {
                anchors.centerIn: parent
                source: Img.builtin("chevron-left").withFillColor(Theme.foreground)
                width: 20
                height: 20
                opacity: settings.canGoBack ? 1 : 0.35
            }

            HoverHandler {
                id: backHover
            }
            TapHandler {
                enabled: settings.canGoBack
                onTapped: settings.goBack()
            }
        }

        Rectangle {
            Layout.preferredWidth: 1
            Layout.topMargin: 6
            Layout.bottomMargin: 6
            Layout.fillHeight: true
            opacity: (backHover.hovered && settings.canGoBack) || (forwardHover.hovered && settings.canGoForward) ? 0 : 1
            color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.12)
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                anchors.fill: parent
                topRightRadius: height / 2
                bottomRightRadius: height / 2
                color: Qt.rgba(Theme.foreground.r, Theme.foreground.g, Theme.foreground.b, 0.07)
                visible: forwardHover.hovered && settings.canGoForward
            }

            ViciImage {
                anchors.centerIn: parent
                source: Img.builtin("chevron-right").withFillColor(Theme.foreground)
                width: 20
                height: 20
                opacity: settings.canGoForward ? 1 : 0.35
            }

            HoverHandler {
                id: forwardHover
            }
            TapHandler {
                enabled: settings.canGoForward
                onTapped: settings.goForward()
            }
        }
    }
}
