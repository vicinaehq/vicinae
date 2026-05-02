import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts

ScrollView {
    id: root

    required property string url
    property string title
    property string description
    property string imageUrl

    clip: true
    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth
        spacing: 0

        Text {
            text: root.url
            color: Theme.textMuted
            font.pointSize: Theme.smallerFontSize
            font.family: "monospace"
            elide: Text.ElideMiddle
            Layout.fillWidth: true
            Layout.margins: 12
            Layout.bottomMargin: 4
        }

        Text {
            visible: root.title !== ""
            text: root.title
            color: Theme.accent
            font.pointSize: Theme.regularFontSize
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            maximumLineCount: 2
            elide: Text.ElideRight
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: 4
        }

        Text {
            visible: root.description !== ""
            text: root.description
            color: Theme.foreground
            font.pointSize: Theme.smallerFontSize
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            maximumLineCount: 4
            elide: Text.ElideRight
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.bottomMargin: 8
        }

        Item {
            visible: root.imageUrl !== ""
            Layout.fillWidth: true
            Layout.leftMargin: 12
            Layout.rightMargin: 12
            Layout.preferredHeight: ogImage.implicitWidth > 0 ? width * (ogImage.implicitHeight / ogImage.implicitWidth) : 0

            Rectangle {
                id: imageMask

                anchors.fill: parent
                radius: 8
                visible: false
                layer.enabled: true
            }

            Image {
                id: ogImage

                anchors.fill: parent
                source: root.imageUrl
                fillMode: Image.PreserveAspectFit
                sourceSize.width: 500
                cache: true
                asynchronous: true
                visible: false
            }

            MultiEffect {
                anchors.fill: ogImage
                source: ogImage
                maskEnabled: true
                maskSource: imageMask
            }

        }

    }

}
