import type { AI } from "./ai";
import type { BrowserExtension } from "./browser";
import type { Form } from "./components";
import type { FileSearch } from "./file-search";
import type { Wallpaper } from "./wallpaper";
import type { WindowManagement } from "./window-management";

export interface LaunchContext {
	[item: string]: any;
}

export interface Arguments {
	[item: string]: any;
}

export declare type LaunchProps<
	T extends {
		arguments?: Arguments;
		draftValues?: Form.Values;
		launchContext?: LaunchContext;
	} = {
		arguments: Arguments;
		draftValues: Form.Values;
		launchContext?: LaunchContext;
	},
> = {
	/**
	 * The type of launch for the command (user initiated or background).
	 */
	launchType: LaunchType;
	/**
	 * Use these values to populate the initial state for your command.
	 */
	arguments: T["arguments"];
	/**
	 * When a user enters the command via a draft, this object will contain the user inputs that were saved as a draft.
	 * Use its values to populate the initial state for your Form.
	 */
	draftValues?: T["draftValues"];
	/**
	 * When the command is launched programmatically via `launchCommand`, this object contains the value passed to `context`.
	 */
	launchContext?: T["launchContext"];
	/**
	 * When the command is launched as a fallback command, this string contains the text of the root search.
	 */
	fallbackText?: string;
};

export enum LaunchType {
	/**
	 * A regular launch through user interaction
	 */
	UserInitiated = "userInitiated",

	/**
	 * Scheduled through an interval and launched from background
	 */
	Background = "background",

	/**
	 * The command was started from the command line, using e.g
	 * `vicinae cmd launch`. `process.cwd()` can be called to get the
	 * working directory in which the command was issued.
	 */
	CommandLine = "commandLine",
}

// every API we can check availability for using environment.canAccess
type AccessCheckable =
	| typeof FileSearch
	| typeof BrowserExtension
	| typeof Wallpaper
	| typeof AI
	| typeof WindowManagement;

export interface Environment {
	/**
	 * The version of the main Raycast app
	 */
	raycastVersion: string;
	/**
	 * The name of the extension owner (if any) or author, as specified in package.json
	 */
	ownerOrAuthorName: string;
	/**
	 * The name of the extension, as specified in package.json
	 */
	extensionName: string;
	/**
	 * The name of the launched command, as specified in package.json
	 */
	commandName: string;
	/**
	 * The mode of the launched command, as specified in package.json
	 */
	commandMode: "no-view" | "view" | "menu-bar";
	/**
	 * The absolute path to the assets directory of the extension.
	 *
	 * @remarks
	 * This directory is used internally to load icons, images, and other bundled assets.
	 */
	assetsPath: string;
	/**
	 * The absolute path for the support directory of an extension. Use it to read and write files related to your extension or command.
	 */
	supportPath: string;
	/**
	 * Indicates whether the command is a development command (vs. an installed command from the Store).
	 */
	isDevelopment: boolean;
	/**
	 * The appearance used by the Raycast application.
	 */
	appearance: "light" | "dark";
	/**
	 * The theme used by the Raycast application.
	 * @deprecated Use `appearance` instead
	 */
	theme: "light" | "dark";
	/**
	 * The text size used by the Raycast application.
	 */
	textSize: "medium" | "large";
	/**
	 * The type of launch for the command (user initiated or background).
	 */
	launchType: LaunchType;
	/**
	 * Returns whether the user has access to the given API.
	 *
	 *  @example
	 * ```typescript
	 * import { BrowserExtension, environment } from "@vicinae/api";
	 *
	 * export default async function Command() {
	 *   if (environment.canAccess(BrowserExtension)) {
	 *   	const tabs = await BrowserExtension.getTabs();
	 *   }
	 * }
	 * ```
	 */
	canAccess(api: AccessCheckable): boolean;

	/**
	 * The Vicinae version. Vicinae extensions should rely on this and ignore `raycastVersion`.
	 */
	vicinaeVersion: {
		tag: string;
		commit: string;
	};

	/**
	 * Whether we run an actual Raycast extension in compatibility mode.
	 * This is used internally to provide Raycast-compatible interfaces.
	 */
	isRaycast: boolean;
}

/**
 * General information about the running extension command, the vicinae version, the capabilities of
 * the system we are running on, etc...
 *
 * @example
 * ```typescript
 * import { environment } from '@vicinae/api';
 *
 * console.log({ environment });
 * ```
 */
import { getGlobal } from "./globals";

export const environment: Environment = getGlobal().environ;
