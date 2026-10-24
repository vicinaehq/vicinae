import { getClient } from "./client";

/**
 * Use browser extension features, provided that the user has it installed and active.
 */
export namespace BrowserExtension {
	export type BookmarkOptions = { title?: string; url?: string };

	export type Tab = {
		id: number;
		title?: string;
		url: string;
		active: boolean;
		browserId: string;
		/** epoch ms when the tab was last focused; 0 = unknown */
		lastAccessed: number;
		pinned: boolean;
		audible: boolean;
		muted: boolean;
		focus: () => void;
		close: () => void;
		setPinned: (pinned: boolean) => Promise<void>;
		setMuted: (muted: boolean) => Promise<void>;
		bookmark: (options?: BookmarkOptions) => Promise<void>;
		duplicate: () => Promise<void>;
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
				setPinned: (pinned) => browser.setPinned(b.browserId, b.id, pinned),
				setMuted: (muted) => browser.setMuted(b.browserId, b.id, muted),
				bookmark: (options) =>
					browser.bookmarkTab(b.browserId, b.id, options?.title, options?.url),
				duplicate: () => browser.duplicateTab(b.browserId, b.id),
			})),
		);
	}

	export async function closeTab(
		browserId: string,
		tabId: number,
	): Promise<void> {
		return getClient().BrowserExtension.closeTab(browserId, tabId);
	}

	export function setPinned(
		browserId: string,
		tabId: number,
		pinned: boolean,
	): Promise<void> {
		return getClient().BrowserExtension.setPinned(browserId, tabId, pinned);
	}

	export function setMuted(
		browserId: string,
		tabId: number,
		muted: boolean,
	): Promise<void> {
		return getClient().BrowserExtension.setMuted(browserId, tabId, muted);
	}

	export function bookmarkTab(
		browserId: string,
		tabId: number,
		options?: BookmarkOptions,
	): Promise<void> {
		return getClient().BrowserExtension.bookmarkTab(
			browserId,
			tabId,
			options?.title,
			options?.url,
		);
	}

	export function duplicateTab(
		browserId: string,
		tabId: number,
	): Promise<void> {
		return getClient().BrowserExtension.duplicateTab(browserId, tabId);
	}
}
