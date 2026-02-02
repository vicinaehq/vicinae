import * as vicinae from '@vicinae/api';
import * as raycast from '@raycast/api';

export namespace Clipboard {
	export const copy: typeof raycast.Clipboard.copy = (content, opts) => {
		return vicinae.Clipboard.copy(content, {
			concealed: opts?.concealed || opts?.transient,
			...opts
		});
	}

	export const paste: typeof raycast.Clipboard.paste = vicinae.Clipboard.paste;
	export const read: typeof raycast.Clipboard.read = vicinae.Clipboard.read;
	export const readText: typeof raycast.Clipboard.readText = vicinae.Clipboard.readText;
	export const clear: typeof raycast.Clipboard.clear = vicinae.Clipboard.clear;
};

