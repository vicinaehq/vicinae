export const themeColors = [
	'blue', 'green', 'magenta', 'orange', 'purple', 'red', 'yellow', 'primary-text', 'secondary-text'
] as const;


export type ThemeColorName = typeof themeColors[number];

const isThemeColor = (s: string): s is ThemeColorName => themeColors.includes(s as any);

export type ThemeColor = { themeColor: ThemeColorName };

export const Color: Record<'Blue' | 'Green' | 'Magenta' | 'Orange'| 'Purple' | 'Red' | 'Yellow' | 'PrimaryText' | 'SecondaryText', ThemeColor>  = {
	Blue: { themeColor: 'blue' },
	Green: { themeColor: 'green' },
	Magenta: { themeColor: 'magenta' },
	Orange: { themeColor: 'orange' },
	Purple: { themeColor: 'purple' },
	Red: { themeColor: 'red' },
	Yellow: { themeColor: 'yellow' },
	PrimaryText: { themeColor: 'primary-text' },
	SecondaryText: { themeColor: 'secondary-text' },
};

export type SerializedColorLike = ThemeColor | { colorString: string; }

export type ColorLike = SerializedColorLike | string;

export const serializeColorLike = (color: ColorLike): SerializedColorLike => {
	if (typeof color == 'string') {
		if (isThemeColor(color)) return { themeColor: color };
		
		return { colorString: color };
	}

	return color;
}
