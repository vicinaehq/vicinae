import { bus } from "./bus";
import { FileInfo as ProtoFileInfo } from "./proto/file-search";

/**
 * Access Vicinae's built-in file search functionality.
 *
 * @remarks
 * This API provides read-only access to the file search system.
 * Use this to find existing files indexed by Vicinae.
 * This does **not** provide indexing capabilities.
 *
 * @example
 * ```typescript
 * import { FileSearch } from '@vicinae/api';
 *
 * const results = await FileSearch.search('invoice');
 * console.log(`Found ${results.length} files`);
 * ```
 *
 * @category File Search
 * @public
 */
export namespace FileSearch {
	/**
	 * For now we provide no options, but in the future we will implement mime type and file type filtering.
	 * */
	export type SearchOptions = {};

	export type FileInfo = ProtoFileInfo;

	/**
	 * Search for files matching the provided query string.
	 *
	 * @param query - Search term (min. 1 character) - the shorter the query the longer the average search takes.
	 * @param options - Search configuration options
	 * @returns Promise resolving to array of matching files
	 *
	 * @remarks
	 * Uses prefix matching on filename tokens. For example:
	 * - File: "invoice-new-motherboard.pdf"
	 * - Matches: "inv", "new", "mother", "pdf"
	 * - No match: "board", "oice" (not prefixes)
	 *
	 * @example
	 * ```typescript
	 * const files = await fileSearch.search('invoice');
	 * ```
	 */
	export async function search(
		query: string,
		_: FileSearch.SearchOptions = {},
	): Promise<FileSearch.FileInfo[]> {
		const res = await bus.request("fileSearch.search", { query });

		return res.unwrap().files;
	}
}
