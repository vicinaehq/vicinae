import QtQuick

Item {
    id: root

    readonly property int _radius: Math.min(Config.borderRounding, 15)
    readonly property alias macImpl: macLoader.item

    readonly property var _window: root.Window.window
    // Qt.Tool contains the Qt.Popup bit, so mask the full window type or the
    // launcher window itself matches for in-scene popups
    readonly property bool _nativeWindow: _window !== null && (_window.flags & Qt.WindowType_Mask) === Qt.Popup

    WindowMaterial.enabled: root._nativeWindow && Config.blurEnabled
    WindowMaterial.radius: _radius

    Loader {
        id: macLoader
        active: Qt.platform.os === "osx"
        source: "qrc:/Vicinae/PopupMaterialMacOS.qml"
    }
}
