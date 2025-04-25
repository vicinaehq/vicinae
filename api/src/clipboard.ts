import { PathLike } from 'fs';
import { bus } from './bus';

type CopyParams = {
};

export namespace Clipboard {
	export type Content = { text: string } | { file: PathLike } | { html: string, text?: string };
	export type ReadContent = { text: string } | { file?: string } | { html?: string };
	export type CopyOptions = {
		concealed?: boolean;
	};
};

export const Clipboard = {
	async copy(text: string | number | Clipboard.Content, options: Clipboard.CopyOptions = {}) {
		let content = typeof text === 'object' ? text : { text: `${text}` };

		await bus.request('clipboard.copy', {
			content,
			options
		});
	},

	async paste(content : string | Clipboard.Content) {
		await bus.request('clipboard.paste', {
			content
		});
	},

	async read(options?: { offset?: number }): Promise<Clipboard.ReadContent> {
		const res = await bus.request<{ content: Clipboard.ReadContent }>('clipboard.read', {
			options
		});

		return res.data.content;
	},

	async readText(options?: { offset?: number }): Promise<string | undefined> {
		const res = await bus.request<{ content?: string }>('clipboard.read-text', {
			options
		});

		return res.data.content;
	},

	async clear(text: string) {
		await bus!.request('clipboard.clear', {
			text
		});
	}
};
