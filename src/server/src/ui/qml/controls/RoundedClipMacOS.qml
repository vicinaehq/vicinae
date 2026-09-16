import QtQuick

ShaderEffect {
    property variant source
    property real cornerRadius: 10
    readonly property vector2d itemSize: Qt.vector2d(width, height)
    fragmentShader: "qrc:/shaders/roundedclip.frag.qsb"
}
