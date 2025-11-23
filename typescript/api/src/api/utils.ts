import type { PathLike } from "node:fs";
import { rm } from "node:fs/promises";
import { bus } from "./bus";
import { WindowManagement } from "./window-management";
import type { Application } from "./proto/application";

/**
  @ignore - we should probably move this to raycast compat, I don't think we want that.
 */
export const trash = async (path: PathLike | PathLike[]): Promise<void> => {
	const targets = Array.isArray(path) ? path : [path];
	const promises = targets.map((p) => rm(p, { recursive: true }));

	await Promise.all(promises);
};

/**
 * Additional options that can be passed in order to tweak the behavior of the terminal window.
 * Note that most options are best-effort, which means that they may or may not work depending on
 * the default terminal emulator that is available.
 * On Linux, Vicinae honors the xdg-terminal-exec specification: https://gitlab.freedesktop.org/terminal-wg/specifications/-/merge_requests/3/diffs
 *
 * @category System
 */
export type RunInTerminalOptions = {
	/**
	 * Ensure the terminal window is held open after the execution of the command completes.
	 * This option is provided as a hint to the default terminal emulator, and might not be honored.
	 */
	hold?: boolean;

	/**
	 * Overrides the application ID used for this specific terminal window.
	 * Can be useful if you want to target this window by its class with the window management API.
	 * This option is provided as a hint to the default terminal emulator, and might not be honored.
	 *
	 * @see WindowManagement
	 */
	appId?: string;

	/**
	 * Overrides the title used for this specific terminal window.
	 * Can be useful if you want to target this window by its title with the window management API.
	 * This option is provided as a hint to the default terminal emulator, and might not be honored.
	 *
	 * @see WindowManagement
	 */
	title?: string;
};

/**
 * Run a command in a new terminal emulator window.
 *
 * @param args - the command line to execute. This is *not* getting interpreted by a shell.
 * @param options - list of options that can be passed in order to tweak the behavior of the terminal window.
 *
 * @example
 * ```typescript
 * await runInTerminal(['journalctl', '--user', '-u', '-f', 'vicinae']);
 * // or, inside a shell:
 * await runInTerminal(['/bin/bash', 'echo "dis is my home: $HOME"'], { hold: true });
 * ```
 *
 * @category System
 */
export const runInTerminal = async (
	args: string[],
	options: RunInTerminalOptions = {},
) => {
	const { hold = false, appId, title } = options;

	await bus.request("app.runInTerminal", {
		cmdline: args,
		hold,
		appId,
		title,
	});
};

/**
 * @category System
 */
export const open = async (target: string, app?: Application | string) => {
	let appId: string | undefined;

	if (app) {
		if (typeof app === "string") {
			appId = app;
		} else {
			appId = app.id;
		}
	}

	await bus.request("app.open", {
		target,
		appId,
	});
};

/**
 * @category System
 */
export const getFrontmostApplication = async (): Promise<Application> => {
	const { application } = await WindowManagement.getActiveWindow();

	if (!application) {
		throw new Error(`Could not get frontmost application`);
	}

	return application;
};

/**
 * @category System
 */
export const getApplications = async (
	target?: string,
): Promise<Application[]> => {
	const res = await bus.request("app.list", { target });

	return res.unwrap().apps;
};

/**
 * @category System
 */
export const getDefaultApplication = async (
	path: string,
): Promise<Application> => {
	const res = await bus.request("app.getDefault", { target: path });
	const app = res.unwrap().app;

	if (!app) throw new Error(`No default application for target ${path}`);

	return app;
};

/**
 * @category System
 */
export const showInFileBrowser = async (path: PathLike): Promise<void> => {
	const fileBrowser = await getDefaultApplication("inode/directory"); // FIXME: we may want something more robust
	await open(path.toString(), fileBrowser);
};

export { Application } from "./proto/application";
