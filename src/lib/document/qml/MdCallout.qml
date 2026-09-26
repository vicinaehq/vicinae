pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Shapes
import QtQuick.Layouts
import Vicinae.Documents

Rectangle {
    id: root
    readonly property DocumentStyle style: root.DocumentScope.style

    property var blockData: ({})
    property string fontFamily: ""
    property real fontSize: root.style.regularFontSize
    property real lineHeight: 1.0

    readonly property string calloutType: blockData.calloutType ?? "note"
    readonly property var paragraphs: blockData.paragraphs ?? []

    readonly property color _alertColor: {
        switch (calloutType) {
        case "caution":
            return root.style.dangerColor;
        case "warning":
            return root.style.warningColor;
        case "important":
            return root.style.warningColor;
        case "tip":
            return root.style.successColor;
        default:
            return root.style.infoColor;
        }
    }

    readonly property string _label: {
        switch (calloutType) {
        case "caution":
            return qsTr("Caution");
        case "warning":
            return qsTr("Warning");
        case "important":
            return qsTr("Important");
        case "tip":
            return qsTr("Tip");
        default:
            return qsTr("Note");
        }
    }

    readonly property string _iconPath: {
        switch (calloutType) {
        case "caution":
        case "warning":
            return "M10.9 2.793c-1.195-2.39-4.605-2.39-5.8 0l-3.755 7.514C.268 12.463 1.835 15 4.245 15h7.51c2.41 0 3.977-2.537 2.9-4.693zm-4.458.67c.642-1.284 2.474-1.284 3.116 0l3.755 7.514a1.743 1.743 0 0 1-1.558 2.523h-7.51a1.743 1.743 0 0 1-1.558-2.523zM8.75 6a.75.75 0 0 0-1.5 0v2a.75.75 0 1 0 1.5 0zM8 10a1 1 0 1 0 0 2 1 1 0 0 0 0-2";
        case "important":
            return "M5.1 2.793c1.195-2.39 4.605-2.39 5.8 0l3.755 7.514c1.077 2.156-.49 4.693-2.9 4.693h-7.51c-2.41 0-3.977-2.537-2.9-4.693zm4.458.67c-.642-1.284-2.474-1.284-3.116 0l-3.755 7.514A1.743 1.743 0 0 0 4.245 13.5h7.51a1.743 1.743 0 0 0 1.558-2.523zM8 5a1 1 0 0 1 1 1v2a1 1 0 0 1-2 0V6a1 1 0 0 1 1-1m0 5a1 1 0 1 0 0 2 1 1 0 0 0 0-2";
        case "tip":
            return "M8 2.5a3.5 3.5 0 0 0-3.09 5.147c.178.33.584.97.889 1.44.272.42.43.908.449 1.413H7.25V6.75a.75.75 0 0 1 1.5 0v3.75h1.002a2.8 2.8 0 0 1 .45-1.413c.304-.47.71-1.11.887-1.44A3.5 3.5 0 0 0 8 2.5M9.75 12h-3.5v.75c0 .215.073.406.186.533.1.113.268.217.564.217h2c.296 0 .464-.104.564-.217a.8.8 0 0 0 .186-.533zM3 6a5 5 0 1 1 9.412 2.353c-.213.399-.657 1.095-.952 1.55-.138.212-.21.455-.21.7v2.147c0 .535-.177 1.094-.564 1.53-.4.449-.982.72-1.686.72H7c-.704 0-1.286-.271-1.686-.72a2.3 2.3 0 0 1-.564-1.53v-2.146c0-.246-.073-.489-.21-.7-.296-.456-.74-1.152-.952-1.55A5 5 0 0 1 3 6";
        default:
            return "M8 2.5a5.5 5.5 0 1 0 0 11 5.5 5.5 0 0 0 0-11M1 8a7 7 0 1 1 14 0A7 7 0 0 1 1 8m7 4a1 1 0 0 0 1-1V8a1 1 0 0 0-2 0v3a1 1 0 0 0 1 1m0-6a1 1 0 1 1 0-2 1 1 0 0 1 0 2";
        }
    }

    width: parent?.width ?? 0
    implicitHeight: layout.implicitHeight + 16
    color: Qt.rgba(_alertColor.r, _alertColor.g, _alertColor.b, 0.15)
    radius: 6

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Shape {
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                preferredRendererType: Shape.CurveRenderer
                ShapePath {
                    strokeWidth: 0
                    fillColor: root._alertColor
                    PathSvg {
                        path: root._iconPath
                    }
                }
            }

            Text {
                text: root._label
                color: root._alertColor
                font.pointSize: root.fontSize
                font.bold: true
                Binding on font.family {
                    value: root.fontFamily
                    when: root.fontFamily !== ""
                }
            }
        }

        Repeater {
            model: root.paragraphs

            DocumentText {
                id: calloutText
                Layout.fillWidth: true
                Layout.leftMargin: 22
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap
                color: root.style.foreground
                fontSize: root.fontSize
                lineHeight: root.lineHeight
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
