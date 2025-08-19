"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.serializeColorLike = exports.Color = exports.themeColors = void 0;
exports.themeColors = [
    'blue', 'green', 'magenta', 'orange', 'purple', 'red', 'yellow', 'primary-text', 'secondary-text'
];
const isThemeColor = (s) => exports.themeColors.includes(s);
exports.Color = {
    Blue: 'blue',
    Green: 'green',
    Magenta: 'magenta',
    Orange: 'orange',
    Purple: 'purple',
    Red: 'red',
    Yellow: 'yellow',
    PrimaryText: 'primary-text',
    SecondaryText: 'secondary-text'
};
const serializeColorLike = (color) => {
    if (typeof color == 'string') {
        if (isThemeColor(color))
            return { themeColor: color };
        return { colorString: color };
    }
    return color;
};
exports.serializeColorLike = serializeColorLike;
