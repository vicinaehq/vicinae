export declare const themeColors: readonly ["blue", "green", "magenta", "orange", "purple", "red", "yellow", "primary-text", "secondary-text"];
export type ThemeColorName = typeof themeColors[number];
export type ThemeColor = {
    themeColor: ThemeColorName;
};
export type Color = typeof themeColors[number];
export declare const Color: Record<'Blue' | 'Green' | 'Magenta' | 'Orange' | 'Purple' | 'Red' | 'Yellow' | 'PrimaryText' | 'SecondaryText', Color>;
export type SerializedColorLike = ThemeColor | {
    colorString: string;
};
export type ColorLike = SerializedColorLike | string;
export declare const serializeColorLike: (color: ColorLike) => SerializedColorLike;
//# sourceMappingURL=color.d.ts.map