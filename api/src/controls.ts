import { bus } from './bus';

export const closeMainWindow = async () => {
	await bus.turboRequest('ui.closeMainWindow', {});
}

export const clearSearchBar = async () => {
	await bus.turboRequest('ui.setSearchText', { text: '' });
}
