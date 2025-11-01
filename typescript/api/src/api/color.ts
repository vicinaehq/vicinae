type DynamicColor = {
	dark: string;
	light: string;
	adjustContrast?: boolean;
};

export namespace Color {
	export type Dynamic = DynamicColor;
	export type Raw = string;
}

export enum Color {
	Blue = "blue",
	Green = "green",
	Magenta = "magenta",
	Orange = "orange",
	Purple = "purple",
	Red = "red",
	Yellow = "yellow",
	PrimaryText = "primary-text",
	SecondaryText = "secondary-text",
}

export type ColorLike = Color.Dynamic | Color.Raw | Color;
export type SerializedColorLike = Color.Dynamic | string;

export const serializeColorLike = (color: ColorLike): SerializedColorLike => {
	if (typeof color == "string") {
		return color;
	}

	return color;
};
