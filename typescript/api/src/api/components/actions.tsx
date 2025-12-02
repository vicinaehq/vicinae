import React, { type ReactNode, useRef } from "react";
import { randomUUID } from "node:crypto";
import type { PathLike } from "node:fs";
import { useNavigation } from "../hooks/index";
import { Clipboard } from "../clipboard";
import { type ImageLike, serializeProtoImage } from "../image";
import type { Keyboard } from "../keyboard";
import { type Application, open, showInFileBrowser } from "../utils";
import type { Form } from "./form";
import { Icon } from "../icon";
import { closeMainWindow } from "../controls";

type BaseActionProps = {
	title: string;
	icon?: ImageLike;
	/**
	 * The shortcut to bind this action to.
	 * It is highly recommended to use named shortcuts such as
	 * Keyboard.Shortcut.Common.Copy as they will respect user preferences.
	 */
	shortcut?: Keyboard.Shortcut | Keyboard.Shortcut.Common;
	autoFocus?: boolean;
	style?: "regular" | "destructive";
};

type ActionProps = BaseActionProps & {
	onAction: () => void;
};

/**
 * Actions that can be used as part of an {@link ActionPanel}
 *
 * Most actions are just simple wrappers around the base {@link Action}. For any custom need, use that.
 *
 * @category Actions
 */
export namespace Action {
	export namespace CopyToClipboard {
		export type Props = Omit<BaseActionProps, "title"> & {
			content: string;
			concealed?: boolean;
			onCopy?: (content: string | number | Clipboard.Content) => void;
			title?: string;
		};
	}

	export namespace Push {
		export type Props = BaseActionProps & {
			target: ReactNode;
		};
	}

	export namespace Open {
		export type Props = BaseActionProps & {
			target: string;
			app?: Application;
		};
	}

	export namespace Paste {
		export type Props = BaseActionProps & {
			content: string;
			onPaste?: (content: string | number | Clipboard.Content) => void;
		};
	}

	export namespace OpenInBrowser {
		export type Props = BaseActionProps & {
			url: string;
		};
	}

	export namespace ShowInFinderProps {
		export type Props = {
			path: PathLike;
			icon?: ImageLike;
			onShow?: (path: PathLike) => void;
			shortcut?: Keyboard.Shortcut;
			title?: string;
		};
	}

	export namespace SubmitForm {
		export type Props = Omit<BaseActionProps, "title"> & {
			onSubmit: (
				input: Form.Values,
			) => boolean | void | Promise<boolean | void>;
			title?: string;
		};
	}

	export namespace CreateQuicklink {
		export type Props = Omit<BaseActionProps, "title"> & {
			title?: string;
			quicklink: Quicklink;
		};
	}
}

export type Quicklink = {
	name?: string;
	link: string;
	application?: string | Application;
	icon?: Icon;
};

const ActionRoot: React.FC<ActionProps> = ({ icon, ...props }) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	const stableIdRef = useRef<string | undefined>(undefined);
	if (!stableIdRef.current) {
		stableIdRef.current = randomUUID();
	}
	return (
		<action {...props} icon={serializedIcon} stableId={stableIdRef.current} />
	);
};

const CopyToClipboard: React.FC<Action.CopyToClipboard.Props> = ({
	title = "Copy to clipboard",
	icon = Icon.CopyClipboard,
	content,
	concealed = false,
	onCopy,
	...props
}) => {
	return (
		<ActionRoot
			title={title}
			{...props}
			icon={Icon.CopyClipboard}
			onAction={async () => {
				Clipboard.copy(content, { concealed });
				closeMainWindow();
				onCopy?.(content);
			}}
		/>
	);
};

const Paste: React.FC<Action.Paste.Props> = ({
	title = "Paste to active window",
	icon = Icon.CopyClipboard,
	content,
	onPaste,
	...props
}) => {
	return (
		<ActionRoot
			title={title}
			{...props}
			icon={Icon.CopyClipboard}
			onAction={async () => {
				closeMainWindow(); // we close before pasting to make sure focus has been properly restored
				Clipboard.paste(content);
				onPaste?.(content);
			}}
		/>
	);
};

const Open: React.FC<Action.Open.Props> = ({ target, app, ...props }) => {
	return (
		<ActionRoot
			{...props}
			onAction={() => {
				open(target, app);
			}}
		/>
	);
};

const OpenInBrowser: React.FC<Action.OpenInBrowser.Props> = ({
	url,
	title = "Open in Browser",
	...props
}) => {
	return (
		<ActionRoot
			{...props}
			title={title}
			onAction={() => {
				open(url);
			}}
		/>
	);
};

const ShowInFinder: React.FC<Action.ShowInFinderProps.Props> = ({
	path,
	title = "Show in Finder",
	...props
}) => {
	return (
		<ActionRoot
			{...props}
			title={title}
			onAction={() => {
				showInFileBrowser(path);
			}}
		/>
	);
};

const Push: React.FC<Action.Push.Props> = ({ target, ...props }) => {
	const { push } = useNavigation();

	return (
		<ActionRoot
			{...props}
			onAction={() => {
				console.log("activate push action");
				push(target);
			}}
		/>
	);
};

const SubmitForm: React.FC<Action.SubmitForm.Props> = ({
	title = "Submit",
	...props
}) => {
	const stableIdRef = useRef<string | undefined>(undefined);
	if (!stableIdRef.current) {
		stableIdRef.current = randomUUID();
	}
	const nativeProps: React.JSX.IntrinsicElements["action"] = {
		...props,
		stableId: stableIdRef.current,
		title,
		icon: props.icon ? serializeProtoImage(props.icon) : props.icon,
		onAction: () => { },
	};

	return <action {...nativeProps} />;
};

const CreateQuicklink: React.FC<Action.CreateQuicklink.Props> = ({
	title = "Create Quicklink",
	quicklink,
	...props
}) => {
	const stableIdRef = useRef<string | undefined>(undefined);
	if (!stableIdRef.current) {
		stableIdRef.current = randomUUID();
	}
	const nativeProps: React.JSX.IntrinsicElements["action"] = {
		...props,
		stableId: stableIdRef.current,
		title,
		type: "create-quicklink",
		quicklink: {
			link: quicklink.link,
			name: quicklink.name,
			application:
				typeof quicklink.application === "string"
					? quicklink.application
					: quicklink.application?.name,
			icon: quicklink.icon,
		},
		icon: props.icon ? serializeProtoImage(props.icon) : props.icon,
		onAction: () => { },
	};

	return <action {...nativeProps} />;
};

/**
 * @category Actions
 */
export const Action = Object.assign(ActionRoot, {
	CopyToClipboard,
	Push,
	Open,
	Paste,
	SubmitForm,
	OpenInBrowser,
	ShowInFinder,
	CreateQuicklink,
	Style: {
		Regular: "regular",
		Destructive: "destructive",
	} satisfies { Regular: "regular"; Destructive: "destructive" },
});
