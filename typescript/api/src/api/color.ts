import * as ui from "./proto/ui";

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
export type SerializedColorLike = ui.ColorLike;

export const serializeColorLike = (color: ColorLike): SerializedColorLike => {
	const colorLike = ui.ColorLike.create();

	if (typeof color === "string") {
		colorLike.raw = color;
	} else {
		// It's a DynamicColor
		const dynamicColor = ui.DynamicColor.create();
		dynamicColor.light = color.light;
		dynamicColor.dark = color.dark;
		if (color.adjustContrast !== undefined) {
			dynamicColor.adjustContrast = color.adjustContrast;
		}
		colorLike.dynamic = dynamicColor;
	}

	return colorLike;
};
