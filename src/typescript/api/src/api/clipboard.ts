import type { PathLike } from "node:fs";
import type { ClipboardContent } from "./proto/api";
import { client } from "./client";

/**
 * Access system clipboard and clipboard history features.
 *
 * @category Clipboard
 */
export namespace Clipboard {
	export type Content =
		| { text: string }
		| { file: PathLike }
		| { html: string; text?: string };

	export type ReadContent = {
		text?: string;
		file?: string;
		html?: string;
	};

	export type CopyOptions = {
		concealed?: boolean;
	};

	function mapContent(
		content: string | number | Clipboard.Content,
	): ClipboardContent {
		let ct: ClipboardContent = {};

		if (typeof content !== "object") {
			ct.text = `${content}`;
		} else {
			if (content["file"]) {
				ct.path = content["file"];
			} else if (content["html"]) {
				ct.html = content["html"];
				ct.text = content["text"];
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
		await client.Clipboard.copy(mapContent(text), {
			concealed: options.concealed ?? false,
		});
	}

	/**
	 * Paste the provided clipboard content to the active window.
	 * If the environment does not support either getting the active window
	 * or pasting content to it directly, this will fallback to a regular
	 * clipboard copy.
	 */
	export async function paste(text: string | number | Clipboard.Content) {
		await client.Clipboard.paste(mapContent(text));
	}

	/**
	 * Read the current content of the clipboard, which can contain text, html and a file path.
	 * Note: the offset option is not yet implemented
	 *
	 * ```ts
	 * const { text, html, file } = await Clipboard.read();
	 * ```
	 */
	export async function read(): Promise<Clipboard.ReadContent> {
		return client.Clipboard.readContent();
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
	export async function readText(): Promise<string> {
		return read().then((r) => r.text ?? "");
	}

	/**
	 * Clear the current clipboard content.
	 */
	export async function clear() {
		await client.Clipboard.clear();
	}
}
