pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Vicinae

ColumnLayout {
    id: root

    property var blockData: ({})
    property string fontFamily: ""
    property real fontSize: Theme.regularFontSize
    property real lineHeight: 1.0
    property bool ordered: false
    property int startNumber: blockData.startNumber ?? 1
    property int depth: 0

    width: parent?.width ?? 0
    spacing: 2

    Repeater {
        model: root.blockData.items ?? []

        ColumnLayout {
            id: itemDelegate
            Layout.fillWidth: true
            spacing: 2

            required property var modelData
            required property int index

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: root.depth * 16
                spacing: 6

                Text {
                    Layout.alignment: Qt.AlignTop
                    Layout.topMargin: 2
                    text: root.ordered ? (root.startNumber + itemDelegate.index) + "." : "\u2022"
                    color: Theme.textMuted
                    font.pointSize: root.fontSize
                    Binding on font.family {
                        value: root.fontFamily
                        when: root.fontFamily !== ""
                    }
                }

                DocumentText {
                    id: itemText
                    Layout.fillWidth: true
                    textFormat: TextEdit.RichText
                    wrapMode: TextEdit.Wrap
                    color: Theme.foreground
                    fontSize: root.fontSize
                    lineHeight: root.lineHeight
                    Binding on font.family {
                        value: root.fontFamily
                        when: root.fontFamily !== ""
                    }
                    text: itemDelegate.modelData.html ?? ""
                    selectionPart: itemDelegate.modelData.selectionPart ?? itemDelegate.index
                }
            }

            // Nested sub-lists — use Loader to break recursive instantiation
            Repeater {
                model: itemDelegate.modelData.children ?? []

                Loader {
                    id: nestedList
                    required property var modelData
                    Layout.fillWidth: true
                    source: "MdList.qml"
                    onLoaded: {
                        item.fontFamily = Qt.binding(function () {
                            return root.fontFamily;
                        });
                        item.fontSize = Qt.binding(() => root.fontSize);
                        item.lineHeight = Qt.binding(() => root.lineHeight);
                        item.ordered = nestedList.modelData.ordered ?? false;
                        item.startNumber = nestedList.modelData.startNumber ?? 1;
                        item.depth = root.depth + 1;
                        item.blockData = {
                            items: nestedList.modelData.items ?? []
                        };
                    }
                }
            }
        }
    }
}
