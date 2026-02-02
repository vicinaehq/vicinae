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
		title: proto.title,
		workspaceId: proto.workspaceId,
		active: proto.active,
		bounds: {
			position: { x: proto.x, y: proto.y },
			size: { width: proto.width, height: proto.height },
		},
		application: proto.app,
		focus() {
			return WindowManagement.focusWindow(this);
		},
	};
};

/**
 * Access Vicinae's window management features.
 *
 * @remarks
 * Window management support varies a lot depending on the environment.
 * Right now it is pretty well supported on almost all linux desktop environments except KDE.
 *
 * @example
 * ```typescript
 * import { WindowManagement } from '@vicinae/api';
 *
 * const wins = await WindowManagement.getWindows();
 * const browserWindow = wins.find(w => w.application?.name?.includes('firefox'));
 *
 * if (browserWindow) {
 *  await browserWindow.focus();
 * }
 * ```
 *
 * @category Window Management
 * @public
 */
export namespace WindowManagement {
	/**
	 * A window as defined by the windowing system in use.
	 * A window can be optionally tied to an application or a workspace.
	 */
	export type Window = {
		id: string;

		title: string;

		/**
		 * Whether this window is currently active.
		 * This is usually the window that currently owns focus.
		 */
		active: boolean;

		bounds: {
			position: { x: number; y: number };
			size: { height: number; width: number };
		};

		/**
		 * The ID of the workspace this window belongs to, if applicable in the context
		 * of the current window manager.
		 */
		workspaceId?: string;

		/**
		 * The application this window belongs to, if any.
		 */
		application?: Application;

		/**
		 * Request that the window manager focuses this window.
		 * @see {@link focusWindow}
		 */
		focus: () => Promise<boolean>;
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
		/**
		 * Name assigned by the windowing system to that screen.
		 *
		 * In Wayland environments for instance, the name is set to something like `DP-1`, `DP-2`, `e-DP1`...
		 *
		 * @remarks The name is not guaranteed to remain stable but is usually stable enough to uniquely identify a screen.
		 */
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

	export async function getWindows(
		options: wm.GetWindowsRequest = {},
	): Promise<WindowManagement.Window[]> {
		const res = await bus.request("wm.getWindows", options);

		return res.unwrap().windows.map(transformWindow);
	}

	/**
	 * Focus `window`.
	 *
	 * @remarks
	 * Window objects have a {@link Window.focus} method that can be used to achieve the same thing on a specific window directly.
	 *
	 * @param window - the window to focus. You may want to make sure this window still exists when you request focus.
	 *
	 * @return `true` if the window was focused, `false` otherwise.
	 * A window may not have been focused because it doesn't accept focus (e.g some layer shell surfaces)
	 * or simply because it doesn't exist anymore.
	 *
	 * @see {@link Window.focus}
	 */
	export async function focusWindow(window: Window): Promise<boolean> {
		const res = await bus.request("wm.focusWindow", {
			id: window.id,
		});

		return res.unwrap().ok;
	}

	/**
	 * Return the list of screens (physical and virtual) currently attached to the computer.
	 */
	export async function getScreens(): Promise<Screen[]> {
		const res = await bus.request("wm.getScreens", {});
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
		const res = await bus.request("wm.getActiveWorkspace", {});

		return transformWorkspace(res.unwrap().workspace!);
	}

	export async function getWorkspaces(): Promise<WindowManagement.Workspace[]> {
		const res = await bus.request("wm.getWorkspaces", {});

		return res.unwrap().workspaces.map(transformWorkspace);
	}

	export async function getWindowsOnActiveWorkspace(): Promise<
		WindowManagement.Window[]
	> {
		const workspace = await getActiveWorkspace();

		return getWindows({ workspaceId: workspace.id });
	}

	export async function setWindowBounds(payload: wm.SetWindowBoundsRequest) {
		await bus.request("wm.setWindowBounds", payload);
	}

	export async function getActiveWindow(): Promise<WindowManagement.Window> {
		const res = await bus.request("wm.getActiveWindow", {});

		return transformWindow(res.unwrap().window!);
	}
}
