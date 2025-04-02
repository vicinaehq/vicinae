import { bus } from './bus';

export const closeMainWindow = async () => {
	await bus.request('close-main-window', {});
}

export const clearSearchBar = async () => {
	await bus.request('clear-search-bar', {});
}
