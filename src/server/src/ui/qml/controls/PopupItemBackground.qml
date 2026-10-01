pragma ComponentBehavior: Bound
import QtQuick
import Vicinae

Loader {
    id: root

    property bool selected: false
    property bool hovered: false
    property bool nativeWindow: false
    property real radius: 0
    property PopupMaterialStyle style: null
    readonly property color selectedText: nativeWindow && style ? style.selectedText : Theme.listItemSelectionFg
    readonly property real backgroundOpacity: nativeWindow ? Config.popupOpacity : 0
    sourceComponent: nativeWindow && style?.itemBackgroundComponent ? style.itemBackgroundComponent : defaultBackground

    Component {
        id: defaultBackground
        SourceBlendRect {
            radius: root.radius
            backgroundColor: Config.withAlpha(Theme.popoverBackground, root.backgroundOpacity)
            color: {
                const fill = root.selected ? Theme.listItemSelectionBg : root.hovered ? Theme.listItemHoverBg : Theme.popoverBackground;
                const alpha = root.selected || root.hovered ? Config.popupSurfaceOpacity : root.backgroundOpacity;
                return Config.withAlpha(fill, alpha);
            }
        }
    }
}
