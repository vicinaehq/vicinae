import React, { ReactNode } from "react";
import { AppInfo, useNavigation } from "../hooks";
import { Clipboard } from "../clipboard";
import { ImageLike, SerializedImageLike, serializeImageLike } from "../image";

export type BaseActionProps = {
	title: string;
	icon?: ImageLike;
}

export type FinalizedActionProps = {
	onAction: () => void;
	icon?: SerializedImageLike;
};

export type ActionProps = BaseActionProps & {
	title: string;
	onAction: () => void
};

export type CopyToClipboardProps = BaseActionProps & {
	content: string;
};

export type ActionPushProps = BaseActionProps & {
	target: ReactNode;
}

export type ActionOpenProps = BaseActionProps & {
	app?: AppInfo;
};

const ActionRoot: React.FC<ActionProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['action'] = {
		title: props.title,
		onAction: props.onAction,
	}

	if (props.icon) {
		nativeProps.icon = serializeImageLike(props.icon);
	}

	return <action {...nativeProps} />
}

const CopyToClipboard: React.FC<CopyToClipboardProps> = ({ content, ...props }) => {
	return <ActionRoot {...props} onAction={() => {
		Clipboard.copy(content);
	}} />
}

const Open: React.FC<ActionOpenProps> = ({ app, ...props }) => {
	return <ActionRoot {...props} onAction={() => {
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
