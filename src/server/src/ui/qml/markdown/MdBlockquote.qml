pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

Item {
    id: root

    property var blockData: ({})
    property string fontFamily: ""

    readonly property var paragraphs: blockData.paragraphs ?? []

    width: parent?.width ?? 0
    implicitHeight: row.implicitHeight

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 3
            Layout.fillHeight: true
            radius: 1.5
            color: Theme.divider
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            spacing: 4

            Repeater {
                model: root.paragraphs

                DocumentText {
                    id: bqText
                    Layout.fillWidth: true
                    textFormat: TextEdit.RichText
                    wrapMode: TextEdit.Wrap
                    color: Theme.textMuted
                    font.pointSize: Theme.regularFontSize
                    font.italic: true
                    Binding on font.family {
                        value: root.fontFamily
                        when: root.fontFamily !== ""
                    }
                    text: modelData ?? ""
                    selectionPart: index

                    required property var modelData
                    required property int index
                }
            }
        }
    }
}
