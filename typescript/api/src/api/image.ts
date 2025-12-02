import { type ColorLike, serializeColorLike } from "./color";
import type { Icon } from "./icon";
import * as ui from "./proto/ui";

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
	| ui.Image
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

const maskMap: Record<Image.Mask, ui.ImageMask> = {
	[Image.Mask.Circle]: ui.ImageMask.Circle,
	[Image.Mask.RoundedRectangle]: ui.ImageMask.RoundedRectangle,
};

export const serializeProtoImage = (image: ImageLike): ui.Image => {
	const serializeSource = (payload: Image.Source): ui.ImageSource => {
		if (typeof payload === "object") {
			const tmp = payload as Image.ThemedSource;

			return {
				themed: { light: tmp.light.toString(), dark: tmp.dark.toString() },
			};
		}

		return { raw: payload.toString() };
	};

	if (image instanceof URL || typeof image === "string") {
		return { source: { raw: image.toString() } };
	}

	const proto = ui.Image.create();
	const img = image as Image;

	// img.source should technically not be null, but it somehow still happens at times with some
	// raycast extensions
	if (img.source) {
		proto.source = serializeSource(img.source);
	}

	if (img.fallback) {
		proto.fallback = serializeSource(img.fallback);
	}

	if (img.mask) {
		proto.mask = maskMap[img.mask];
	}

	if (img.tintColor) {
		proto.tintColor = serializeColorLike(img.tintColor);
	}

	return proto;
};
