import { ReactNode } from "react";
import { Image, ImageLike, serializeImageLike } from "../image";
import { Keyboard } from "../keyboard";
import { useEventListener } from "../hooks";

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

const ActionPanelSection: React.FC<React.PropsWithChildren<ActionPanelSectionProps>> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['action-panel-section'] = {
		title: props.title,
	}

	return (
		<action-panel-section {...nativeProps}>
			{props.children}
		</action-panel-section>
	);
}

export type ActionPanelSubmenuProps = {
	title: string;
	icon?: Image.ImageLike;
	shortcut?: Keyboard.Shortcut;
	onOpen?: () => void;
	onSearchTextChange?: (text: string) => void
	children: ReactNode;
};

const ActionPannelSubmenu: React.FC<ActionPanelSubmenuProps> = ({ icon, children, onOpen, onSearchTextChange, ...props }) => {
	const onSearchTextChangeHandler = useEventListener(onSearchTextChange);
	const onOpenHandler = useEventListener(onOpen);
	const nativeProps: React.JSX.IntrinsicElements['action-panel-submenu'] = props;

	nativeProps.onSearchTextChange = onSearchTextChangeHandler; 
	nativeProps.onOpen = onOpenHandler;

	if (icon) nativeProps.icon = serializeImageLike(icon);

	return <action-panel-submenu {...nativeProps}>
		{children}
	</action-panel-submenu>

}

export const ActionPanel = Object.assign(ActionPanelRoot, {
	Section: ActionPanelSection,
	Submenu: ActionPannelSubmenu,
});
