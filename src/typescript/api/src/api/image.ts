import { type ColorLike, serializeColorLike } from "./color";
import { Icon } from "./icon";
import type * as api from "./proto/api";

/**
 * Representation of an image, renderable everywhere Vicine expects them.
 * @category Image
 */
export type Image = {
	source: Image.Source;
	fallback?: Image.Fallback | undefined | null;
	tintColor?: ColorLike | undefined | null;
	mask?: Image.Mask | undefined | null;
};

/**
 * @category Image
 */
export type ImageLike = Image.ImageLike; // TODO: FileIcon

export type SerializedImageLike =
	| URL
	| Image.Asset
	| Icon
	| api.Image
	| Image.ThemedImage;

/**
 * @category Image
 */
export namespace Image {
	export type Asset = string;
	export type ThemedSource = { light: URL | Asset; dark: URL | Asset };
	export type Fallback = Source;
	export type Source = URL | Asset | ThemedSource;
	export type ThemedImage = { light: URL | Asset; dark: URL | Asset };
	export type ImageLike = URL | Image.Asset | Icon | Image | Image.ThemedImage;

	export enum Mask {
		Circle = "circle",
		RoundedRectangle = "roundedRectangle",
	}
}

const maskMap: Record<Image.Mask, api.ImageMask> = {
	[Image.Mask.Circle]: "Circle",
	[Image.Mask.RoundedRectangle]: "RoundedRectangle",
};

export const serializeProtoImage = (image: ImageLike): api.Image => {
	const serializeSource = (payload: Image.Source): api.ImageSource => {
		if (typeof payload === "object") {
			const tmp = payload as Image.ThemedSource;

			return {
				themed: { light: tmp.light?.toString(), dark: tmp.dark?.toString() },
			};
		}

		return { raw: payload.toString() };
	};

	if (image instanceof URL || typeof image === "string") {
		return { source: { raw: image.toString() } };
	}

	const img = image as Image;

	// img.source should technically not be null, but it somehow still happens at times with some
	// raycast extensions
	if (!img.source) return serializeProtoImage(Icon.QuestionMarkCircle);

	return {
		source: serializeSource(img.source),
		fallback: img.fallback ? serializeSource(img.fallback) : undefined,
		mask: img.mask ? maskMap[img.mask] : undefined,
		tintColor: img.tintColor ? serializeColorLike(img.tintColor) : undefined,
	};
};
