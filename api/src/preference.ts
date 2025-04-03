import { bus } from './bus';

export const getPreferenceValues = <T = { [preferenceName: string]: any }>(): T => {
	return {} as any;
}

export const openExtensionPreferences = async (): Promise<void> => {
	await bus.request('open-extension-preferences');
}

export const openCommandPreferences = async (): Promise<void> => {
	await bus.request('open-command-preferences');
}
