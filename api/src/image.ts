export type SerializedImageLike = {
	url: string;
} | {
	theme?: string;
	iconName: string;
} | {
	path: string;
}

export type ImageLike = SerializedImageLike | URL;

export const serializeImageLike = (image: ImageLike): SerializedImageLike => {
	if (image instanceof URL) {
		return { url: image.toString() };
	}

	return image;
}
