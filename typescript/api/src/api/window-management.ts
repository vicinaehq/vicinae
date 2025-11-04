import { bus } from "./bus";
import { Application } from "./proto/application";
import * as wm from "./proto/wm";

const transformWorkspace = (
	proto: wm.Workspace,
): WindowManagement.Workspace => {
	return {
		id: proto.id,
		name: proto.name,
		active: proto.active,
		monitorId: proto.monitor,
	};
};

const transformWindow = (proto: wm.Window): WindowManagement.Window => {
	return {
		id: proto.id,
		workspaceId: proto.workspaceId,
		active: proto.active,
		bounds: {
			position: { x: proto.x, y: proto.y },
			size: { width: proto.width, height: proto.height },
		},
		application: proto.app,
	};
};

/**
 * Access Vicinae's window management features.
 *
 * @remarks
 * Window management features are available to a different degree depending on what environment vicinae runs
 * in.
 *
 * @example
 * ```typescript
 * import { WindowManagement } from '@vicinae/api';
 *
 * const windows = await WindowManagement.getWindows();
 * ```
 *
 * @public
 */
export namespace WindowManagement {
	export type Window = {
		id: string;
		active: boolean;
		bounds: {
			position: { x: number; y: number };
			size: { height: number; width: number };
		};
		workspaceId?: string;
		application?: Application;
	};

	export type Workspace = {
		id: string;
		name: string;
		monitorId: string;
		active: boolean;
	};

	/**
	 * A screen, physical or virtual, attached to this computer.
	 */
	export type Screen = {
		name: string;
		/**
		 * Name of the screen's manufacturer.
		 */
		make: string;
		model: string;
		/**
		 * The serial number of the screen, if available.
		 */
		serial?: string;
		bounds: {
			position: { x: number; y: number };
			size: { width: number; height: number };
		};
	};

	export async function ping() {
		const res = await bus.turboRequest("wm.ping", {});
		return res.unwrap().ok;
	}

	export async function getWindows(
		options: wm.GetWindowsRequest = {},
	): Promise<WindowManagement.Window[]> {
		const res = await bus.turboRequest("wm.getWindows", options);

		return res.unwrap().windows.map(transformWindow);
	}

	/**
	 * Return the list of screens (physical and virtual) currently attached to the computer.
	 */
	export async function getScreens(): Promise<Screen[]> {
		const res = await bus.turboRequest("wm.getScreens", {});
		return res.unwrap().screens.map<Screen>((sc) => ({
			name: sc.name,
			make: sc.make,
			model: sc.model,
			serial: sc.serial,
			bounds: {
				position: { x: sc.x, y: sc.y },
				size: { width: sc.width, height: sc.height },
			},
		}));
	}

	export async function getActiveWorkspace(): Promise<WindowManagement.Workspace> {
		const res = await bus.turboRequest("wm.getActiveWorkspace", {});

		return transformWorkspace(res.unwrap().workspace!);
	}

	export async function getWorkspaces(): Promise<WindowManagement.Workspace[]> {
		const res = await bus.turboRequest("wm.getWorkspaces", {});

		return res.unwrap().workspaces.map(transformWorkspace);
	}

	export async function getWindowsOnActiveWorkspace(): Promise<
		WindowManagement.Window[]
	> {
		const workspace = await getActiveWorkspace();

		return getWindows({ workspaceId: workspace.id });
	}

	export async function setWindowBounds(payload: wm.SetWindowBoundsRequest) {
		await bus.turboRequest("wm.setWindowBounds", payload);
	}

	export async function getActiveWindow(): Promise<WindowManagement.Window> {
		const res = await bus.turboRequest("wm.getActiveWindow", {});

		return transformWindow(res.unwrap().window!);
	}
}
