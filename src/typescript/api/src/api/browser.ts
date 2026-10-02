import { getClient } from "./client";

/**
 * Use browser extension features, provided that the user has it installed and active.
 */
export namespace BrowserExtension {
	export type Tab = {
		id: number;
		title?: string;
		url: string;
		active: boolean;
		browserId: string;
		focus: () => void;
		close: () => void;
	};

	export async function getTabs(): Promise<Tab[]> {
		const browser = getClient().BrowserExtension;
		return browser.getTabs().then((bb) =>
			bb.map<Tab>((b) => ({
				...b,
				focus: () => {
					getClient().BrowserExtension.focusTab(b.browserId, b.id);
				},
				close: () => {
					getClient().BrowserExtension.closeTab(b.browserId, b.id);
				},
			})),
		);
	}

	export async function closeTab(
		browserId: string,
		tabId: number,
	): Promise<void> {
		return getClient().BrowserExtension.closeTab(browserId, tabId);
	}
}
