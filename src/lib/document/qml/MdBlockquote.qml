pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae.Documents

Item {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style

    property var blockData: ({})
    property string fontFamily: ""
    property real fontSize: root.style.regularFontSize
    property real lineHeight: 1.0

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
            color: root.style.divider
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
                    color: root.style.textMuted
                    fontSize: root.fontSize
                    lineHeight: root.lineHeight
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
