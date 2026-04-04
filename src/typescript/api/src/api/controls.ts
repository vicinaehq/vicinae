import { getClient } from "./client";
import type * as api from "./proto/api";

/**
 * @category Launcher Window
 */
export enum PopToRootType {
	/**
	 * Translates to Immediate or Suspended depending on the
	 * user configuration.
	 */
	Default = "default",
	/**
	 * Immediately pop to root
	 */
	Immediate = "immediate",
	/**
	 * Do not pop to root, preserve the navigation state as it is
	 */
	Suspended = "suspended",
}

const popToRootProtoMap: Record<PopToRootType, api.PopToRootType> = {
	[PopToRootType.Default]: "Default",
	[PopToRootType.Immediate]: "Immediate",
	[PopToRootType.Suspended]: "Suspended",
};

/**
 * Close the window and show a small HUD where the window was previously opened.
 * Note that the HUD may not be shown in some environments.
 * Similarly to `closeWindow`, it is possible to override the pop to root behavior directly
 * by passing options to this function.
 *
 * @see closeWindow
 *
 * @category Launcher Window
 */
export const showHUD = async (
	title: string,
	options?: { clearRootSearch?: boolean; popToRootType?: PopToRootType },
) => {
	getClient().UI.showHud(
		title,
		options?.clearRootSearch ?? false,
		popToRootProtoMap[options?.popToRootType ?? PopToRootType.Default],
	);
};

/**
 * Close the vicinae launcher window immediately.
 * It is possible to override the `popToRoot` behavior defined in the settings using the options object.
 *
 * @category Launcher Window
 */
export const closeMainWindow = async (
	options: { clearRootSearch?: boolean; popToRootType?: PopToRootType } = {},
) => {
	const { clearRootSearch = false, popToRootType = PopToRootType.Default } =
		options;

	await getClient().UI.closeMainWindow(
		clearRootSearch,
		popToRootProtoMap[popToRootType],
	);
};

/**
 * @category Launcher Window
 */
export const clearSearchBar = async () => {
	await getClient().UI.setSearchText("");
};

/**
 * Get the text that is currently selected by the user.
 * How this is implemented depends on the environment but all it does is usually
 * read the clipboard's primary selection buffer.
 *
 * @category Launcher Window
 */
export const getSelectedText = async () => {
	return getClient().UI.getSelectedText();
};

/**
 * Pop to the root of the navigation stack, optionally clearing the search bar.
 *
 * @category Launcher Window
 */
export const popToRoot = async (options?: { clearSearchBar?: boolean }) => {
	await getClient().UI.popToRoot(options?.clearSearchBar ?? false);
};
