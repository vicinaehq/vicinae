import QtQuick
import QtQuick.Layouts

SelectableDelegate {
    id: root
    height: 90

    required property string calcQuestion
    required property string calcQuestionUnit
    required property string calcAnswer
    required property string calcAnswerUnit

    Item {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        Column {
            id: leftColumn
            anchors.left: parent.left
            anchors.right: arrowIcon.left
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                id: questionText
                width: parent.width
                text: root.calcQuestion
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize * 1.5
                font.weight: Font.Medium
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                width: parent.width
                text: root.calcQuestionUnit || "Question"
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            width: 1
            anchors.top: parent.top
            anchors.bottom: arrowIcon.top
            anchors.bottomMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.divider
        }

        ViciImage {
            id: arrowIcon
            width: 20
            height: 20
            anchors.centerIn: parent
            source: Img.builtin("arrow-right").withFillColor(Theme.textMuted)
        }

        Rectangle {
            width: 1
            anchors.top: arrowIcon.bottom
            anchors.topMargin: 4
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.divider
        }

        Column {
            anchors.left: arrowIcon.right
            anchors.leftMargin: 12
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                width: parent.width
                text: root.calcAnswer
                color: root.selected ? Theme.listItemSelectionFg : Theme.accent
                font.pointSize: Theme.regularFontSize * 1.5
                font.weight: Font.DemiBold
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                width: parent.width
                text: root.calcAnswerUnit || "Answer"
                color: Theme.textMuted
                font.pointSize: Theme.smallerFontSize
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
