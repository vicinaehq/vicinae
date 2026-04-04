import { getClient } from "./client";
import { closeMainWindow } from "./controls";
import { getGlobal } from "./globals";

export const getPreferenceValues = <
	T = { [preferenceName: string]: any },
>(): T => {
	return getGlobal().preferences;
};

/**
 * Opens the extension preferences in the vicinae settings window.
 */
export const openExtensionPreferences = async (): Promise<void> => {
	console.error("openExtensionPreferences is not implemented");
	await getClient().Command.openExtensionPreferences();
	await closeMainWindow();
};

/**
 * Open the command preferences in the vicinae settings window.
 */
export const openCommandPreferences = async (): Promise<void> => {
	await getClient().Command.openCommandPreferences();
	await closeMainWindow();
};
