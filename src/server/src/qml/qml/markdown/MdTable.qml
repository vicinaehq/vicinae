import QtQuick
import QtQuick.Layouts
import Vicinae

Rectangle {
    id: root

    property var blockData: ({})
    property var mdModel: null
    property int blockIndex: -1
    property var selectionController: null
    property string fontFamily: ""

    readonly property int columnCount: blockData.columnCount ?? 0
    readonly property var alignments: blockData.alignments ?? []
    readonly property var headers: blockData.headers ?? []
    readonly property var rows: blockData.rows ?? []

    width: parent?.width ?? 0
    implicitHeight: tableCol.implicitHeight
    color: "transparent"
    border.width: 1
    border.color: Theme.divider
    radius: 4
    clip: true

    function textAlignment(colIdx) {
        var a = alignments[colIdx] ?? 0
        if (a === 1) return Text.AlignHCenter
        if (a === 2) return Text.AlignRight
        return Text.AlignLeft
    }

    ColumnLayout {
        id: tableCol
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: headerRow.implicitHeight
            color: Theme.secondaryBackground
            radius: root.radius
            visible: root.headers.length > 0

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: parent.radius
                color: parent.color
            }

            RowLayout {
                id: headerRow
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: root.headers

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.minimumWidth: 40
                        color: "transparent"
                        implicitHeight: headerText.implicitHeight + 16

                        Rectangle {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: index < root.columnCount - 1 ? 1 : 0
                            color: Theme.divider
                        }

                        TextEdit {
                            id: headerText
                            anchors.fill: parent
                            anchors.margins: 8
                            readOnly: true
                            selectionColor: Theme.textSelectionBg
                            selectedTextColor: Theme.textSelectionFg
                            textFormat: TextEdit.RichText
                            wrapMode: TextEdit.Wrap
                            color: Theme.foreground
                            font.pointSize: Theme.regularFontSize
                            font.bold: true
                            Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }
                            horizontalAlignment: root.textAlignment(index)
                            text: modelData.html ?? ""

                            Component.onCompleted: if (root.selectionController) root.selectionController.registerSelectable(headerText, root.blockIndex * 10000 + index, true)
                            Component.onDestruction: if (root.selectionController) root.selectionController.unregisterSelectable(headerText)
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.divider
            visible: root.headers.length > 0
        }

        Repeater {
            model: root.rows

            Column {
                Layout.fillWidth: true
                required property var modelData
                required property int index

                readonly property int rowIdx: index

                RowLayout {
                    width: parent.width
                    spacing: 0

                    Repeater {
                        model: modelData

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 40
                            color: "transparent"
                            implicitHeight: cellText.implicitHeight + 16

                            required property int index
                            required property var modelData

                            Rectangle {
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: index < root.columnCount - 1 ? 1 : 0
                                color: Theme.divider
                            }

                            TextEdit {
                                id: cellText
                                anchors.fill: parent
                                anchors.margins: 8
                                readOnly: true
                                selectionColor: Theme.textSelectionBg
                                selectedTextColor: Theme.textSelectionFg
                                textFormat: TextEdit.RichText
                                wrapMode: TextEdit.Wrap
                                color: Theme.foreground
                                font.pointSize: Theme.regularFontSize
                                Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }
                                horizontalAlignment: root.textAlignment(index)
                                text: modelData.html ?? ""

                                Component.onCompleted: if (root.selectionController) root.selectionController.registerSelectable(cellText, root.blockIndex * 10000 + 100 + rowIdx * root.columnCount + index, true)
                                Component.onDestruction: if (root.selectionController) root.selectionController.unregisterSelectable(cellText)
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: Theme.divider
                    visible: index < root.rows.length - 1
                }
            }
        }
    }
}
