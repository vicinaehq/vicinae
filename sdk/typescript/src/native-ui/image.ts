import { NativeElement } from './types';

export type ImageProps = {
	path?: string;
	width?: number;
	height?: number;
};

export class Image extends NativeElement<ImageProps> {
	constructor(props: ImageProps = {}) {
		super(Image.name, props, []);
	}

	size(n: number) {
		return this.width(n).height(n);
	}

	width(n: number) {
		return this.buildProp('width', n);
	}

	height(n: number) {
		return this.buildProp('height', n);
	}
};

export class LocalImage extends Image {
	constructor(path: string) {
		super({ path });
	}
}
