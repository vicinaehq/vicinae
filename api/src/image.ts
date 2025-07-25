import { Color } from "./color";

export type SerializedImageLike = Image | { fileIcon: string; } | { light: URL | Image.Asset, dark: URL | Image.Asset };

export type FileIcon = {
	fileIcon: string;
};

export type Image = {
	source: Image.Source;
	fallback?: Image.Fallback;
	tintColor?: Color
	mask?: Image.Mask;
};


export type ImageLike = Image.ImageLike;

export namespace Image {
	export type Asset = string;
	export type ThemedSource = { light: URL | Asset, dark: URL | Asset };
	export type Fallback = Asset | ThemedSource;
	export type Source = URL | Asset | ThemedSource;
	export type ImageLike = URL | Image.Asset |  FileIcon | Image | ThemedImage;
	export type ThemedImage = { light: URL | Asset, dark: URL | Asset };

	export enum Mask {
		Circle = 'circle',
		RoundedRectangle = 'roundedRectangle'
	};
};


export const serializeImageLike = (image: Image.ImageLike): SerializedImageLike => {
	if (image instanceof URL) {
		return { source: image.toString() };
	}

	if (typeof image == "string") {
		return { source: image };
	}

	return image;
}
