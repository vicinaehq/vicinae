import { Color } from "./color";
export type SerializedImageLike = Image | {
    fileIcon: string;
};
export type FileIcon = {
    fileIcon: string;
};
export type Image = {
    source: Image.Source;
    fallback?: Image.Fallback;
    tintColor?: Color;
    mask?: Image.Mask;
};
export type ImageLike = Image.ImageLike;
export declare namespace Image {
    type Asset = string;
    type ThemedSource = {
        light: URL | Asset;
        dark: URL | Asset;
    };
    type Fallback = Asset | ThemedSource;
    type Source = URL | Asset | ThemedSource;
    type ImageLike = URL | Image.Asset | FileIcon | Image;
    enum Mask {
        Circle = "circle",
        RoundedRectangle = "roundedRectangle"
    }
}
export declare const serializeImageLike: (image: Image.ImageLike) => SerializedImageLike;
//# sourceMappingURL=image.d.ts.map