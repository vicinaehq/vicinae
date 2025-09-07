import { bus } from './bus';
import * as wm from './proto/wm';

export namespace WindowManagement {
	export type Window = wm.Window;
};

/**
 */
class WindowManagementImpl {
	async ping() {
		const res = await bus.turboRequest('wm.ping', {});
		return res.unwrap().ok;
	}

	async getWindows(options: wm.GetWindowsRequest = {}) {
		const res = await bus.turboRequest('wm.getWindows', options);

		return res.unwrap().windows;
	}

	async getActiveWorkspace() {
		const res = await bus.turboRequest('wm.getActiveWorkspace', {});

		return res.unwrap().workspace!;
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

		return res.unwrap().window!;
	}
};

export const WindowManagement = new WindowManagementImpl();
