import { PathLike } from 'fs';
import { bus } from './bus';
import { ClipboardContent } from './proto/clipboard';

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
	mapContent(content: string | number | Clipboard.Content): ClipboardContent {
		let ct = ClipboardContent.create();

		if (typeof content != 'object') {
			ct.text = `${content}`;
		} else {
			if (content['file']) {
				ct.path = { path: content['file'] };
			} else if (content['html']) {
				ct.html = {html: content['html'], text: content['text']}
			} else {
				ct.text = content['text'];
			}
		}

		return ct;
	},

	async copy(text: string | number | Clipboard.Content, options: Clipboard.CopyOptions = {}) {
		await bus.turboRequest('clipboard.copy', {
			content: this.mapContent(text),
			options: { concealed: options.concealed ?? false }
		});
	},

	async paste(content : string | Clipboard.Content) {
		// TODO: implement
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
		// TODO: implement
	}
};
