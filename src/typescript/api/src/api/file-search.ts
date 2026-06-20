import { getClient } from "./client";
import type * as api from "./proto/api";
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

	export type FileInfo = api.FileInfo;

	export interface FileSearchOptions {
		/**
		 * The maximum number of files to return in one call.
		 * Defaults to 100, limited to 10,000.
		 */
		limit?: number;
		filters?: {
			/**
			 * Category of files the results should belong to.
			 */
			category?: api.FileSearchCategory;
		};
	}

	/**
	 * Search for files matching the provided query string.
	 *
	 * @param query - Search term (min. 1 character) - the shorter the query the longer the average search takes.
	 * @param options - Search configuration options
	 * @returns Promise resolving to array of matching files
	 *
	 * @remarks
	 * Uses fuzzy filename matching backed by Vicinae's file index. For example:
	 * - File: "example folder"
	 * - Matches: "fol exa", "exa fol", "ex fold"
	 *
	 * @example
	 * ```typescript
	 * const files = await fileSearch.search('invoice');
	 * ```
	 */
	export async function search(
		query: string,
		{ filters = {}, limit = 100 }: FileSearchOptions = {},
	): Promise<FileSearch.FileInfo[]> {
		return getClient().FileSearch.search(query, { limit, filters });
	}
}
