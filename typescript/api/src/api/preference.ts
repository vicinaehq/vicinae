import { workerData } from "worker_threads";

export const getPreferenceValues = <
	T = { [preferenceName: string]: any },
>(): T => {
	const { preferenceValues = {} } = workerData;

	return preferenceValues;
};

/**
 * TODO: implement
 * @ignore
 */
export const openExtensionPreferences = async (): Promise<void> => {
	console.error("openExtensionPreferences is not implemented");
};

/**
 * TODO: implement
 * @ignore
 */
export const openCommandPreferences = async (): Promise<void> => {
	console.error("openCommandPreferences is not implemented");
};
