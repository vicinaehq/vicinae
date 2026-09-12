pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root
    required property IntroViewHost host

    MarkdownText {
        visible: !root.host.centered
        anchors.fill: parent
        markdown: root.host.introMarkdown
        contentPadding: 20
    }

    ColumnLayout {
        visible: root.host.centered
        anchors.top: parent.top
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(520, parent.width - 40)
        spacing: 12

        ViciImage {
            source: root.host.icon
            Layout.preferredWidth: 56
            Layout.preferredHeight: 56
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: root.host.title
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize * 1.6
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Text {
            text: root.host.description
            color: Theme.textMuted
            font.pointSize: Theme.regularFontSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
