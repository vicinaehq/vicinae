import { Color } from "./color";
import * as ui from "./proto/ui";

export type Image = {
	source: Image.Source;
	fallback?: Image.Fallback;
	tintColor?: Color;
	mask?: Image.Mask;
};

export type ImageLike = Image.ImageLike;

export namespace Image {
	export type Asset = string;
	export type ThemedSource = { light: URL | Asset; dark: URL | Asset };
	export type Fallback = Source;
	export type Source = URL | Asset | ThemedSource;
	export type ImageLike = URL | Image.Asset | Image | ThemedImage;
	export type ThemedImage = { light: URL | Asset; dark: URL | Asset };

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

	proto.source = serializeSource(img.source);

	if (img.fallback) {
		proto.fallback = serializeSource(img.fallback);
	}

	if (img.mask) {
		proto.mask = maskMap[img.mask];
	}

	return proto;
};
