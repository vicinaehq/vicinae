pragma Singleton
import QtQuick
import Vicinae

QtObject {
    function tint(color: color, opacity: real): color {
        if (!Config.blurEnabled || opacity <= 0 || opacity >= 1)
            return Config.withAlpha(color, opacity);

        // Counter the material's neutral wash before compositing. These reference
        // tones are approximate: the live backdrop still contributes to the surface.
        const neutral = Theme.isDark ? 0.20 : 0.92;
        const wash = neutral * (1 - opacity);
        return Qt.rgba(Math.max(0, Math.min(1, (color.r - wash) / opacity)), Math.max(0, Math.min(1, (color.g - wash) / opacity)), Math.max(0, Math.min(1, (color.b - wash) / opacity)), opacity);
    }
}
