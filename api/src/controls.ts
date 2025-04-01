import { bus } from './bus';

export const closeMainWindow = () => {
	bus.request('close-main-window', {});
}
