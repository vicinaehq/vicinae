import React, { ReactNode } from "react";
import { useNavigation } from "../hooks/index";
import { Clipboard } from "../clipboard";
import { ImageLike, serializeImageLike } from "../image";
import { Keyboard } from "../keyboard";
import { Application, open } from "../utils";

export type BaseActionProps = {
	title: string;
	icon?: ImageLike;
	shortcut?: Keyboard.Shortcut;
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

const ActionRoot: React.FC<ActionProps> = ({ icon, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['action'] = props;

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

const Push: React.FC<ActionPushProps> = ({ target, ...props }) => {
	const { push } = useNavigation();

	return <ActionRoot {...props} onAction={() => {
		console.log('activate push action');
		push(target)
	}} />
}


export const Action = Object.assign(ActionRoot, {
	CopyToClipboard,
	Push,
	Open
});
