// deprecated pre-1.0 exports: https://developers.raycast.com/misc/migration

import {
	Action,
	AI,
	Alert,
	Form,
	getPreferenceValues,
	Image,
	List,
	LocalStorage,
	Toast,
} from "@vicinae/api";
import { Clipboard } from "./clipboard.js";
import { ActionPanel } from "./components/action-panel.js";
import { unsupported } from "./unsupported.js";

export const ToastStyle = Toast.Style;
export const ImageMask = Image.Mask;
export const AlertActionStyle = Alert.ActionStyle;

export const ListItem = List.Item;
export const ListSection = List.Section;

export const ActionPanelItem = ActionPanel.Item;
export const ActionPanelSection = ActionPanel.Section;
export const ActionPanelSubmenu = ActionPanel.Submenu;

export const CopyToClipboardAction = Action.CopyToClipboard;
export const OpenAction = Action.Open;
export const OpenInBrowserAction = Action.OpenInBrowser;
export const OpenWithAction = Action.OpenWith;
export const PasteAction = Action.Paste;
export const PushAction = Action.Push;
export const ShowInFinderAction = Action.ShowInFinder;
export const SubmitFormAction = Action.SubmitForm;
export const TrashAction = Action.Trash;

export const FormCheckbox = Form.Checkbox;
export const FormDatePicker = Form.DatePicker;
export const FormDropdown = Form.Dropdown;
export const FormDropdownItem = Form.Dropdown.Item;
export const FormDropdownSection = Form.Dropdown.Section;
export const FormSeparator = Form.Separator;
export const FormTagPicker = Form.TagPicker;
export const FormTagPickerItem = Form.TagPicker.Item;
export const FormTextArea = Form.TextArea;
export const FormTextField = Form.TextField;

export const copyTextToClipboard = Clipboard.copy;
export const pasteText = Clipboard.paste;
export const clearClipboard = Clipboard.clear;

export const allLocalStorageItems = LocalStorage.allItems;
export const getLocalStorageItem = LocalStorage.getItem;
export const setLocalStorageItem = LocalStorage.setItem;
export const removeLocalStorageItem = LocalStorage.removeItem;
export const clearLocalStorage = LocalStorage.clear;
export type LocalStorageValues = LocalStorage.Values;
export type LocalStorageValue = LocalStorage.Value;

export const unstable_AI = AI;
export const useUnstableAI = unsupported("useUnstableAI");
export const useActionPanel = unsupported("useActionPanel");
export const useId = unsupported("useId");
export const render = unsupported("render");
// requires globals to be populated before this module loads (see worker.tsx)
export const preferences = Object.fromEntries(
	Object.entries(getPreferenceValues<Record<string, unknown>>()).map(
		([name, value]) => [name, { name, value }],
	),
);

export const specialKeys = {
	return: "return",
	delete: "delete",
	deleteForward: "deleteForward",
	tab: "tab",
	arrowUp: "arrowUp",
	arrowDown: "arrowDown",
	arrowLeft: "arrowLeft",
	arrowRight: "arrowRight",
	pageUp: "pageUp",
	pageDown: "pageDown",
	home: "home",
	end: "end",
	space: "space",
	escape: "escape",
	enter: "enter",
	backspace: "backspace",
};
