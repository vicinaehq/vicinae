import { bus } from './bus';

type CopyParams = {
};

export const Clipboard = {
	async copy(text: string, params: CopyParams = {}) {
		await bus!.request('clipboard-copy', {
			text
		});
	},

	async clear(text: string) {
		await bus!.request('clipboard-clear', {
			text
		});
	}
};
