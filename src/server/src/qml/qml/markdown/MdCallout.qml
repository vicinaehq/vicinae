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

    readonly property string calloutType: blockData.calloutType ?? "note"
    readonly property var paragraphs: blockData.paragraphs ?? []

    readonly property color _alertColor: {
        switch (calloutType) {
        case "caution": return Theme.toastDanger
        case "warning": return Theme.toastWarning
        case "important": return Theme.toastWarning
        case "tip": return Theme.toastSuccess
        default: return Theme.toastInfo
        }
    }

    readonly property string _label: {
        switch (calloutType) {
        case "caution": return "Caution"
        case "warning": return "Warning"
        case "important": return "Important"
        case "tip": return "Tip"
        default: return "Note"
        }
    }

    readonly property var _iconSource: {
        switch (calloutType) {
        case "caution":
        case "warning":
            return Img.builtin("warning").withFillColor(root._alertColor)
        case "important":
            return Img.builtin("important-01").withFillColor(root._alertColor)
        case "tip":
            return Img.builtin("light-bulb").withFillColor(root._alertColor)
        default:
            return Img.builtin("info-01").withFillColor(root._alertColor)
        }
    }

    width: parent?.width ?? 0
    implicitHeight: _layout.implicitHeight + 16
    color: Qt.rgba(_alertColor.r, _alertColor.g, _alertColor.b, 0.15)
    radius: 6

    ColumnLayout {
        id: _layout
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            ViciImage {
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                source: root._iconSource
            }

            Text {
                text: root._label
                color: root._alertColor
                font.pointSize: Theme.regularFontSize
                font.bold: true
                Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }
            }
        }

        Repeater {
            model: root.paragraphs

            TextEdit {
                id: calloutText
                Layout.fillWidth: true
                Layout.leftMargin: 22
                readOnly: true
                selectionColor: Theme.textSelectionBg
                selectedTextColor: Theme.textSelectionFg
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap
                color: Theme.foreground
                font.pointSize: Theme.regularFontSize
                Binding on font.family { value: root.fontFamily; when: root.fontFamily !== "" }
                text: modelData ?? ""

                required property var modelData
                required property int index

                Component.onCompleted: if (root.selectionController) root.selectionController.registerSelectable(calloutText, root.blockIndex * 10000 + index, true)
                Component.onDestruction: if (root.selectionController) root.selectionController.unregisterSelectable(calloutText)
            }
        }
    }
}
