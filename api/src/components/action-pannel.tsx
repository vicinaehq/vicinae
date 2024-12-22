import { ReactNode } from "react";
import { ImageLike, serializeImageLike } from "../image";

export type ActionPanelProps = {
	title?: string;
	children?: ReactNode;
};

const ActionPanelRoot: React.FC<ActionPanelProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['action-panel'] = props;

	return (
		<action-panel {...nativeProps} />
	);
}

export type ActionPanelSectionProps = {
	title?: string;
	children?: ReactNode;
};

const ActionPanelSection: React.FC<ActionPanelSectionProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['action-panel-section'] = props;

	return (
		<action-panel-section {...nativeProps} />
	);
}

export type ActionPanelSubmenuProps = {
	title: string;
	icon?: ImageLike;
	onOpen?: () => void;
	onSearchTextChange: (text: string) => void
};

const ActionPannelSubmenu: React.FC<ActionPanelSubmenuProps> = ({ icon, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['action-panel-submenu'] = props;

	if (icon) nativeProps.icon = serializeImageLike(icon);

	return <action-panel-submenu {...nativeProps} />
}

export const ActionPannel = Object.assign(ActionPanelRoot, {
	Section: ActionPanelSection,
	Submenu: ActionPannelSubmenu,
});
