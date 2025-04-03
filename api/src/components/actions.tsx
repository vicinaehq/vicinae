import React, { ReactNode } from "react";
import { useNavigation } from "../hooks/index";
import { Clipboard } from "../clipboard";
import { ImageLike, serializeImageLike } from "../image";
import { Keyboard } from "../keyboard";
import { Application, open } from "../utils";
import { useEventListener } from "../hooks";
import { Form } from "./form";

export type BaseActionProps = {
	title: string;
	icon?: ImageLike;
	shortcut?: Keyboard.Shortcut;
	autoFocus?: boolean;
	style?: 'regular' | 'destructive'
}

export type ActionProps = BaseActionProps & {
	onAction: () => void;
};

export type CopyToClipboardProps = BaseActionProps & {
	content: string;
};

export type ActionPushProps = BaseActionProps & {
	target: ReactNode;
}

export type ActionOpenProps = BaseActionProps & {
	target: string;
	app?: Application;
};

export type ActionOpenInBrowserProps = BaseActionProps & {
	url: string;
};

export type ActionSubmitFormProps = Omit<BaseActionProps, 'title'> &  {
	onSubmit: (input: Form.Values) => boolean | void | Promise<boolean | void> ;
	title?: string;
};

const ActionRoot: React.FC<ActionProps> = ({ icon, ...props }) => {
	const handler = useEventListener(props.onAction);
	const nativeProps: React.JSX.IntrinsicElements['action'] = {
		...props,
		icon,
		onAction: handler!
	};

	if (icon) {
		nativeProps.icon = serializeImageLike(icon);
	}

	return <action {...nativeProps} />
}

const CopyToClipboard: React.FC<CopyToClipboardProps> = ({ content, ...props }) => {
	return <ActionRoot {...props} onAction={() => {
		Clipboard.copy(content);
	}} />
}

const Open: React.FC<ActionOpenProps> = ({ target, app, ...props }) => {
	return <ActionRoot {...props} onAction={() => {
		open(target, app);
	}} />
}

const OpenInBrowser: React.FC<ActionOpenInBrowserProps> = ({ url, ...props }) => {
	return <ActionRoot {...props} onAction={() => {
		open(url);
	}} />
}

const Push: React.FC<ActionPushProps> = ({ target, ...props }) => {
	const { push } = useNavigation();

	return <ActionRoot {...props} onAction={() => {
		console.log('activate push action');
		push(target)
	}} />
}

const SubmitForm: React.FC<ActionSubmitFormProps> = ({ onSubmit, ...props }) => {
	return <ActionRoot {...props} title="Submit" onAction={() => {
		onSubmit({});
		console.log('submit form');
	}} />
}


export const Action = Object.assign(ActionRoot, {
	CopyToClipboard,
	Push,
	Open,
	SubmitForm,
	OpenInBrowser,
	Style: {
		Regular: 'regular',
		Destructive: 'destructive'
	} satisfies { Regular: 'regular', Destructive: 'destructive' }
});
