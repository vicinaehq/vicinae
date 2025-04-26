import { bus } from './bus';

export const closeMainWindow = async () => {
	await bus.request('ui.close-main-window', {});
}

export const clearSearchBar = async () => {
	await bus.request('ui.clear-search-bar', {});
}
