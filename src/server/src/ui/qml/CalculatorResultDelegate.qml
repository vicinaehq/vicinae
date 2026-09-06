import QtQuick
import QtQuick.Layouts

SelectableDelegate {
    id: root
    height: 110

    required property string calcQuestion
    required property string calcQuestionUnit
    required property string calcAnswer
    required property string calcAnswerUnit

    Accessible.role: Accessible.ListItem
    Accessible.name: root.calcQuestion
    Accessible.description: root.calcAnswerUnit !== "" ? `${root.calcAnswer} ${root.calcAnswerUnit}` : root.calcAnswer
    Accessible.selectable: true
    Accessible.selected: root.selected

    // quite naive, but good enough.
    // if we find we need this to be actually accurate, we probably need a per backend
    // way to figure out what token carries operator significance.
    function highlight(expr) {
        const escaped = expr.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
        const tokens = /(\b(?:to|in|as|of|mod|and|or|xor)\b|[+\-*\/^%()=,])/gi;
        return escaped.replace(tokens, m => `<font color="${Theme.textMuted}">${m}</font>`);
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        Item {
            id: leftColumn
            anchors.left: parent.left
            anchors.right: arrowIcon.left
            anchors.rightMargin: 12
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Text {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -10
                text: root.highlight(root.calcQuestion)
                textFormat: Text.StyledText
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize * 1.5
                font.weight: Font.Medium
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }

            TextBadge {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                width: Math.min(implicitWidth, parent.width)
                text: root.calcQuestionUnit || qsTr("Expression")
                contentColor: root.selected ? Theme.listItemSelectionFg : Theme.foreground
            }
        }

        Rectangle {
            visible: root.selected
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
            visible: root.selected
            width: 1
            anchors.top: arrowIcon.bottom
            anchors.topMargin: 4
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.divider
        }

        Item {
            id: rightColumn
            anchors.left: arrowIcon.right
            anchors.leftMargin: 12
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Text {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: -10
                text: root.calcAnswer
                color: root.selected ? Theme.listItemSelectionFg : Theme.foreground
                font.pointSize: Theme.regularFontSize * 1.5
                font.weight: Font.DemiBold
                elide: Text.ElideRight
                maximumLineCount: 1
                horizontalAlignment: Text.AlignHCenter
            }

            TextBadge {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                width: Math.min(implicitWidth, parent.width)
                text: root.calcAnswerUnit || qsTr("Result")
                contentColor: root.selected ? Theme.listItemSelectionFg : Theme.foreground
            }
        }
    }
}
