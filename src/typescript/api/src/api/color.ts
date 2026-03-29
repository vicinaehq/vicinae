import type * as api from "./proto/api";

/**
 * @category Colors
 */
export namespace Color {
	export type Dynamic = api.DynamicColor;
	export type Raw = string;
}

/**
 * @category Colors
 */
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

/**
 * @category Colors
 */
export type ColorLike = Color.Dynamic | Color.Raw | Color;

export type SerializedColorLike = api.ColorLike;

export const serializeColorLike = (color: ColorLike): api.ColorLike => {
	if (typeof color === "string") {
		return { raw: color };
	}

	return {
		dynamic: {
			light: color.light,
			dark: color.dark,
			adjustContrast: color.adjustContrast,
		},
	};
};
