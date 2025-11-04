import React, { ReactNode } from "react";
import type { PathLike } from "node:fs";
import { useNavigation } from "../hooks/index";
import { Clipboard } from "../clipboard";
import { ImageLike } from "../image";
import { Keyboard } from "../keyboard";
import { Application, open, showInFileBrowser } from "../utils";
import { Form } from "./form";
import { Icon } from "../icon";
import { closeMainWindow } from "../controls";

export type BaseActionProps = {
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

export type ActionProps = BaseActionProps & {
	onAction: () => void;
};

export type CopyToClipboardProps = Omit<BaseActionProps, "title"> & {
	content: string;
	concealed?: boolean;
	onCopy?: (content: string | number | Clipboard.Content) => void;
	title?: string;
};

export type ActionPushProps = BaseActionProps & {
	target: ReactNode;
};

export type ActionOpenProps = BaseActionProps & {
	target: string;
	app?: Application;
};

export type ActionPasteProps = BaseActionProps & {
	content: string;
	onPaste?: (content: string | number | Clipboard.Content) => void;
};

export type ActionOpenInBrowserProps = BaseActionProps & {
	url: string;
};

export type ActionShowInFinderProps = {
	path: PathLike;
	icon?: ImageLike;
	onShow?: (path: PathLike) => void;
	shortcut?: Keyboard.Shortcut;
	title?: string;
};

export type ActionSubmitFormProps = Omit<BaseActionProps, "title"> & {
	onSubmit: (input: Form.Values) => boolean | void | Promise<boolean | void>;
	title?: string;
};

export type Quicklink = {
	name?: string;
	link: string;
	application?: string | Application;
	icon?: Icon;
};

export type ActionCreateQuicklinkProps = Omit<BaseActionProps, "title"> & {
	title?: string;
	quicklink: Quicklink;
};

const ActionRoot: React.FC<ActionProps> = (props) => {
	return <action {...props} />;
};

const CopyToClipboard: React.FC<CopyToClipboardProps> = ({
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

const Paste: React.FC<ActionPasteProps> = ({
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

const Open: React.FC<ActionOpenProps> = ({ target, app, ...props }) => {
	return (
		<ActionRoot
			{...props}
			onAction={() => {
				open(target, app);
			}}
		/>
	);
};

const OpenInBrowser: React.FC<ActionOpenInBrowserProps> = ({
	url,
	...props
}) => {
	return (
		<ActionRoot
			{...props}
			onAction={() => {
				open(url);
			}}
		/>
	);
};

const ShowInFinder: React.FC<ActionShowInFinderProps> = ({
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

const Push: React.FC<ActionPushProps> = ({ target, ...props }) => {
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

const SubmitForm: React.FC<ActionSubmitFormProps> = ({
	title = "Submit",
	...props
}) => {
	const nativeProps: React.JSX.IntrinsicElements["action"] = {
		...props,
		title,
		onAction: () => {},
	};

	return <action {...nativeProps} />;
};

const CreateQuicklink: React.FC<ActionCreateQuicklinkProps> = ({
	title = "Create Quicklink",
	quicklink,
	...props
}) => {
	const nativeProps: React.JSX.IntrinsicElements["action"] = {
		...props,
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
		onAction: () => {},
	};

	return <action {...nativeProps} />;
};

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
