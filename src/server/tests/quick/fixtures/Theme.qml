pragma Singleton
import QtQuick

QtObject {
    readonly property color foreground: "white"
    readonly property color accent: "blue"
    readonly property color textSelectionBg: "blue"
    readonly property color scrollBarBackground: "gray"
    readonly property int smallerFontSize: 12
    readonly property string fontFamily: Qt.application.font.family
    readonly property string monoFontFamily: "monospace"
}
