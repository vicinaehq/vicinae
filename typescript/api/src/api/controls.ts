import { bus } from "./bus";
import * as ui from "./proto/ui";

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

const popToRootProtoMap: Record<PopToRootType, ui.PopToRootType> = {
	[PopToRootType.Default]: ui.PopToRootType.PopToRootDefault,
	[PopToRootType.Immediate]: ui.PopToRootType.PopToRootImmediate,
	[PopToRootType.Suspended]: ui.PopToRootType.PopToRootSuspended,
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
	bus.request("ui.showHud", {
		text: title,
		clearRootSearch: options?.clearRootSearch ?? false,
		popToRoot:
			popToRootProtoMap[options?.popToRootType ?? PopToRootType.Default],
	});
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

	await bus.request("ui.closeMainWindow", {
		clearRootSearch,
		popToRoot: popToRootProtoMap[popToRootType],
	});
};

/**
 * @category Launcher Window
 */
export const clearSearchBar = async () => {
	await bus.request("ui.setSearchText", { text: "" });
};

/**
 * Get the text that is currently selected by the user.
 * How this is implemented depends on the environment but all it does is usually
 * read the clipboard's primary selection buffer.
 *
 * @category Launcher Window
 */
export const getSelectedText = async () => {
	const response = await bus.request("ui.getSelectedText", {});

	if (!response.ok) {
		throw new Error(`Failed to get selected text`);
	}

	return response.value.text;
};

/**
 * Pop to the root of the navigation stack, optionally clearing the search bar.
 *
 * @category Launcher Window
 */
export const popToRoot = async (options?: { clearSearchBar?: boolean }) => {
	await bus.request("ui.popToRoot", {
		clearSearchBar: options?.clearSearchBar ?? false,
	});
};
