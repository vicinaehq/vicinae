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
        spacing: 32

        Item {
            implicitHeight: 8
        }

        Text {
            text: "Buttons"
            color: Theme.foreground
            font.pointSize: Theme.regularFontSize + 4
            font.bold: true
        }

        ButtonShowcase {
            Layout.fillWidth: true
        }

        Item {
            implicitHeight: 8
        }
    }
}
