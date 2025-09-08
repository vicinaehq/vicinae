import { bus } from './bus';
import { Application } from './proto/application';
import * as wm from './proto/wm';

export namespace WindowManagement {
	export type Window = {
		id: string;
		active: boolean;
		bounds: { position: { x: number; y: number }; size: { height: number; width: number } };
		workspaceId?: string;
		application?: Application;
	};

	export type Workspace = {
		id: string;
		name: string;
		monitorId: string;
		active: boolean;
	};
};


const transformWorkspace = (proto: wm.Workspace): WindowManagement.Workspace => {
	return {
		id: proto.id,
		name: proto.name,
		active: proto.active,
		monitorId: proto.monitor
	};
};

const transformWindow = (proto: wm.Window): WindowManagement.Window => {
	return {
		id: proto.id,
		workspaceId: proto.workspaceId,
		active: proto.active,
		bounds: { position: { x: proto.x, y: proto.y }, size: { width: proto.width, height: proto.height } },
		application: proto.app
	};
};

/**
 */
class WindowManagementImpl {
	async ping() {
		const res = await bus.turboRequest('wm.ping', {});
		return res.unwrap().ok;
	}

	async getWindows(options: wm.GetWindowsRequest = {}): Promise<WindowManagement.Window[]> {
		const res = await bus.turboRequest('wm.getWindows', options);

		return res.unwrap().windows.map(transformWindow);
	}

	async getActiveWorkspace(): Promise<WindowManagement.Workspace> {
		const res = await bus.turboRequest('wm.getActiveWorkspace', {});

		return transformWorkspace(res.unwrap().workspace!);
	}

	async getWorkspaces(): Promise<WindowManagement.Workspace[]> {
		const res = await bus.turboRequest('wm.getWorkspaces', {});

		return res.unwrap().workspaces.map(transformWorkspace);
	}

	async getWindowsOnActiveWorkspace(): Promise<WindowManagement.Window[]> {
		const workspace = await this.getActiveWorkspace();

		return this.getWindows({ workspaceId: workspace.id });
	}

	async setWindowBounds(payload: wm.SetWindowBoundsRequest) {
		await bus.turboRequest('wm.setWindowBounds', payload);
	}

	async getActiveWindow(): Promise<WindowManagement.Window> {
		const res = await bus.turboRequest('wm.getActiveWindow', {});

		return transformWindow(res.unwrap().window!);
	}
};

export const WindowManagement = new WindowManagementImpl();
