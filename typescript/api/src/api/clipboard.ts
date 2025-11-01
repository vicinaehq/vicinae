import { PathLike } from "fs";
import { bus } from "./bus";
import { ClipboardContent } from "./proto/clipboard";

export namespace Clipboard {
	export type Content =
		| { text: string }
		| { file: PathLike }
		| { html: string; text?: string };

	export type ReadContent = {
		text: string;
		file?: string;
		html?: string;
	};

	export type CopyOptions = {
		concealed?: boolean;
	};

	function mapContent(
		content: string | number | Clipboard.Content,
	): ClipboardContent {
		let ct = ClipboardContent.create();

		if (typeof content != "object") {
			ct.text = `${content}`;
		} else {
			if (content["file"]) {
				ct.path = { path: content["file"] };
			} else if (content["html"]) {
				ct.html = { html: content["html"], text: content["text"] };
			} else {
				ct.text = content["text"];
			}
		}

		return ct;
	}

	/**
	 * Copy the provided content in the clipboard.
	 * The `concealed` option can be passed so that the created clipboard selection
	 * does not get indexed by the Vicinae clipboard manager.
	 */
	export async function copy(
		text: string | number | Clipboard.Content,
		options: Clipboard.CopyOptions = {},
	) {
		await bus.turboRequest("clipboard.copy", {
			content: mapContent(text),
			options: { concealed: options.concealed ?? false },
		});
	}

	/**
	 * Paste the provided clipboard content to the active window.
	 * If the environment does not support either getting the active window
	 * or pasting content to it directly, this will fallback to a regular
	 * clipboard copy.
	 */
	export async function paste(text: string | Clipboard.Content) {
		await bus.turboRequest("clipboard.paste", {
			content: mapContent(text),
		});
	}

	/**
	 * Read the current content of the clipboard, which can contain text, html and a file path.
	 * Note: the offset option is not yet implemented
	 *
	 * ```ts
	 * const { text, html, file } = await Clipboard.read();
	 * ```
	 */
	export async function read(options?: {
		offset?: number;
	}): Promise<Clipboard.ReadContent> {
		const res = await bus.turboRequest("clipboard.readContent", {});
		return res.unwrap().content!;
	}

	/**
	 * Read the text representation of the current clipboard data. If the data is not text at all, this
	 * returns an empty string.
	 * If you want to read optional html or file path, consider @see {Clipboard.read}
	 * Note: the offset option is not yet implemented.
	 *
	 * ```ts
	 * const text = await Clipboard.readText();
	 * ```
	 */
	export async function readText(options?: {
		offset?: number;
	}): Promise<string> {
		const { text } = await read(options);
		return text;
	}

	/**
	 * Clear the current clipboard content.
	 */
	export async function clear() {
		await bus.turboRequest("clipboard.clear", {});
	}
}
