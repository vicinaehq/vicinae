import { Application } from '../proto/application';
import { WindowManagement as WM } from '../window-management';

type RaycastWindow = {
	active: boolean;
	bounds: { position: { x: number; y: number }; size: { height: number; width: number } } | "fullscreen";
	desktopId: string;
	fullScreenSettable: boolean;
	id: string;
	positionable: boolean;
	resizable: boolean;
	application?: Application;
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

const transformNativeDesktop = (win: WM.Workspace): RaycastDesktop => {
	return {
		id: win.id,
		screenId: win.monitorId,
		size: { width: 0, height: 0 }, // FIXME: implement
		active: win.active,
		type: DesktopType.User
	};
}

const transformNativeWindow = (win: WM.Window): RaycastWindow => {
	return {
		id: win.id,
		fullScreenSettable: true,
		positionable: true,
		resizable: true,
		active: win.active,
		bounds: win.bounds,
		desktopId: win.workspaceId ?? '0',
		application: win.application
	};
}

class RaycastWindowManagement {
	async getActiveWindow(): Promise<RaycastWindow> {
		const window = await WM.getActiveWindow();

		return transformNativeWindow(window);
	}

	async getDesktops(): Promise<RaycastDesktop[]> {
		const workspaces = await WM.getWorkspaces();

		return workspaces.map(transformNativeDesktop);
	}

	async getWindowsOnActiveDesktop(): Promise<RaycastWindow[]> {
		const wins = await WM.getWindowsOnActiveWorkspace();

		return wins.map(transformNativeWindow);
	}

	constructor() {}
};

export const WindowManagement = new RaycastWindowManagement;
