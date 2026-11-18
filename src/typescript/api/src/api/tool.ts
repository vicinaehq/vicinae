import type { Action } from "./components/actions";
import type { ImageLike } from "./image";

/**
 * @category AI
 */
export namespace Tool {
	/**
	 * Export a function of this type named `confirmation` from a tool to ask the user before the tool runs.
	 * Vicinae does not show the returned details yet: the tool always asks for approval.
	 *
	 * @example
	 * ```typescript
	 * export const confirmation: Tool.Confirmation<Input> = async (input) => ({
	 *   style: Action.Style.Destructive,
	 *   message: "Are you sure you want to delete the file?",
	 *   info: [{ name: "Path", value: input.path }],
	 * });
	 * ```
	 */
	export type Confirmation<T> = (input: T) => Promise<
		| undefined
		| {
				/** @defaultValue `Action.Style.Regular` */
				style?: (typeof Action.Style)[keyof typeof Action.Style];
				/** Name/value pairs describing the side effects of the tool. Pairs without a value are not shown. */
				info?: { name: string; value?: string }[];
				/** Usually a question for the user, e.g. "Are you sure you want to delete the file?" */
				message?: string;
				/** An image representing the side effects of the tool. */
				image?: ImageLike;
		  }
	>;
}
