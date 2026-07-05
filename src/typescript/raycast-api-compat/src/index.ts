/**
 * Raycast compatible entrypoint.
 * We mostly re-export @vicinae/api stuff but it is expected that @vicinae/api will
 * become more and more independent overtime, not strictly following what Raycast does
 * but remaining compatible with it.
 */

export {
	Action,
	AI,
	Alert,
	Cache,
	Color,
	ColorLike,
	clearSearchBar,
	closeMainWindow,
	confirmAlert,
	Detail,
	Environment,
	environment,
	Form,
	Grid,
	getPreferenceValues,
	getSelectedText,
	Icon,
	Image,
	ImageLike,
	Keyboard,
	KeyModifier,
	LaunchType,
	List,
	LocalStorage,
	OAuth,
	open,
	openCommandPreferences,
	openExtensionPreferences,
	PopToRootType,
	popToRoot,
	showHUD,
	showInFileBrowser,
	showToast,
	Toast,
	trash,
	updateCommandMetadata,
	useNavigation,
} from "@vicinae/api";
export { Clipboard } from "./clipboard.js";
export { BrowserExtension } from "./browser.js";
export { ActionPanel } from "./components/action-panel.js";
export * from "./legacy.js";
export {
	captureException,
	getApplications,
	getDefaultApplication,
	getFrontmostApplication,
	getSelectedFinderItems,
	showInFinder,
} from "./system.js";
export {
	launchCommand,
	MenuBarExtra,
	unsupported,
} from "./unsupported.js";
export { randomId } from "./utils.js";
export { WindowManagement } from "./window-management.js";
