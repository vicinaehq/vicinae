import { PathLike } from 'fs';
import { rm } from 'fs/promises';
import { bus } from './bus';

export const captureException = (exception: unknown): void => {
	// maybe one day, if we have a developer hub, also...
	console.error("captureException called on", exception);
}

// Linux systems usually do not have a trash, but maybe we should support one...
export const trash = async (path: PathLike | PathLike[]): Promise<void> => {
	const targets = Array.isArray(path) ? path : [path];
	const promises = targets.map((p) => rm(p, { recursive: true }));

	await Promise.all(promises);
}

export const open = async (target: string, app?: Application | string) => {
	let appId: string | undefined;

	if (app) {
		if (typeof app === 'string') {
			appId = app;
		} else { 
			appId = app.id;
		}
	}

	await bus?.request('apps.open', {
		target,
		appId
	});
}

export type Application = {
	id: string;
	name: string;
	icon: { iconName: string };
};

type MessageApp = { id: string, name: string, icon: string };

const deserializeApp = (app: MessageApp): Application => ({ id: app.id, name: app.name, icon: { iconName: app.icon } });

export const getFrontmostApplication = async (): Promise<Application> => {
	const res = await bus.request<{ app: MessageApp | null }>('apps.get-frontmost');

	if (!res.data.app) {
		throw new Error('couldnt get frontmost app');
	}

	return deserializeApp(res.data.app);
}

export const getApplications = async (path?: PathLike): Promise<Application[]> => {
	const res = await bus.request<{ apps: MessageApp[] }>('apps.list', {
		target: path?.toString()
	});

	return res.data.apps.map(deserializeApp);
}

export const getDefaultApplication = async (path: PathLike): Promise<Application> => {
	const res = await bus.request<{ app: MessageApp | null }>('apps.get-default', {
		target: path.toString()
	});

	if (!res.data.app) {
		throw new Error('couldnt get default app');
	}

	return deserializeApp(res.data.app);
}

export const showInFinder = async (path: PathLike): Promise<void> => {
	await bus.request('show-in-finder');
}
