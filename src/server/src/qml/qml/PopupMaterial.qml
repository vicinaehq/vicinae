import QtQuick

Item {
    id: root

    property bool active: true

    readonly property int _radius: Math.min(Config.borderRounding, 15)
    readonly property alias macImpl: macLoader.item

    WindowMaterial.enabled: root.active && Config.blurEnabled
    WindowMaterial.radius: _radius

    Loader {
        id: macLoader
        active: root.active && Qt.platform.os === "osx"
        source: "qrc:/Vicinae/PopupMaterialMacOS.qml"
    }
}
