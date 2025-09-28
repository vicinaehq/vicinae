import { RaycastApplication, transformApp } from './system';
import * as vicinae from '@vicinae/api';

type RaycastWindow = {
	active: boolean;
	bounds: { position: { x: number; y: number }; size: { height: number; width: number } } | "fullscreen";
	desktopId: string;
	fullScreenSettable: boolean;
	id: string;
	positionable: boolean;
	resizable: boolean;
	application?: RaycastApplication;
};

enum DesktopType {
	User,
	FullScreen
};

type RaycastDesktop = {
	id: string;
	screenId: string;
	size: { height: number; width: number };
	active: boolean;
	type: DesktopType;
};

const transformNativeDesktop = (win: vicinae.WindowManagement.Workspace): RaycastDesktop => {
	return {
		id: win.id,
		screenId: win.monitorId,
		size: { width: 0, height: 0 }, // FIXME: implement
		active: win.active,
		type: DesktopType.User
	};
}

const transformNativeWindow = (win: vicinae.WindowManagement.Window): RaycastWindow => {
	return {
		id: win.id,
		fullScreenSettable: true,
		positionable: true,
		resizable: true,
		active: win.active,
		bounds: win.bounds,
		desktopId: win.workspaceId ?? '0',
		application: win.application && transformApp(win.application)
	};
}

class RaycastWindowManagement {
	async getActiveWindow(): Promise<RaycastWindow> {
		const window = await vicinae.WindowManagement.getActiveWindow();

		return transformNativeWindow(window);
	}

	async getDesktops(): Promise<RaycastDesktop[]> {
		const workspaces = await vicinae.WindowManagement.getWorkspaces();

		return workspaces.map(transformNativeDesktop);
	}

	async getWindowsOnActiveDesktop(): Promise<RaycastWindow[]> {
		const wins = await vicinae.WindowManagement.getWindowsOnActiveWorkspace();

		return wins.map(transformNativeWindow);
	}

	constructor() {}
};

export const WindowManagement = new RaycastWindowManagement;
