import QtQuick

Item {
    id: root

    property bool active: true

    readonly property int _radius: Math.min(Config.borderRounding, 15)

    WindowMaterial.enabled: root.active && Config.blurEnabled
    WindowMaterial.radius: _radius

    Loader {
        active: root.active && Qt.platform.os === "osx"
        source: "qrc:/Vicinae/PopupMaterialMacOS.qml"
    }
}
