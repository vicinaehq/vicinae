import { ReactNode } from "react";
import { Image } from "../image";
import { Keyboard } from "../keyboard";

export type ActionPanelProps = {
	title?: string;
	children?: ReactNode;
};

const ActionPanelRoot: React.FC<ActionPanelProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements["action-panel"] = props;

	return <action-panel {...nativeProps} />;
};

export type ActionPanelSectionProps = {
	title?: string;
	children?: ReactNode;
};

const ActionPanelSection: React.FC<
	React.PropsWithChildren<ActionPanelSectionProps>
> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements["action-panel-section"] = {
		title: props.title,
	};

	return (
		<action-panel-section {...nativeProps}>
			{props.children}
		</action-panel-section>
	);
};

export type ActionPanelSubmenuProps = {
	title: string;
	icon?: Image.ImageLike;
	shortcut?: Keyboard.Shortcut;
	onOpen?: () => void;
	onSearchTextChange?: (text: string) => void;
	children: ReactNode;
};

const ActionPannelSubmenu: React.FC<ActionPanelSubmenuProps> = ({
	children,
	...props
}) => {
	return <action-panel-submenu {...props}>{children}</action-panel-submenu>;
};

export const ActionPanel = Object.assign(ActionPanelRoot, {
	Section: ActionPanelSection,
	Submenu: ActionPannelSubmenu,
});
