pragma Singleton
import QtQuick

QtObject {
    readonly property bool blurEnabled: true
    readonly property string popupMaterial: "liquid_glass"

    function withAlpha(color: color, alpha: real): color {
        return Qt.rgba(color.r, color.g, color.b, alpha);
    }
}
