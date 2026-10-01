import type * as raycast from "@raycast/api";
import * as vicinae from "@vicinae/api";
import { unsupported } from "./unsupported";

export namespace BrowserExtension {
	export const getTabs: typeof raycast.BrowserExtension.getTabs = async () => {
		return vicinae.BrowserExtension.getTabs();
	};

	export const getContent: typeof raycast.BrowserExtension.getContent =
		unsupported("getContent");
}
