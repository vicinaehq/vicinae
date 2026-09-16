import QtQuick

ShaderEffect {
    property variant source
    required property real topInset
    required property real bottomInset
    readonly property real contentHeight: height

    fragmentShader: "qrc:/shaders/scrollfade.frag.qsb"
}
