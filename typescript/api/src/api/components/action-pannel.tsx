import type { ReactNode } from "react";
import type { Keyboard } from "../keyboard";
import { type Image, serializeProtoImage } from "../image";

/**
 * @category Actions
 */
export namespace ActionPanel {
	export type Props = {
		title?: string;
		children?: ReactNode;
	};

	export namespace Section {
		export type Props = {
			title?: string;
			children?: ReactNode;
		};
	}

	export namespace Submenu {
		export type Props = {
			title: string;
			icon?: Image.ImageLike;
			shortcut?: Keyboard.Shortcut;
			autoFocus?: boolean;
			filtering?: boolean | { keepSectionOrder: boolean };
			isLoading?: boolean;
			throttle?: boolean;
			onOpen?: () => void;
			onSearchTextChange?: (text: string) => void;
			children: ReactNode;
		};
	}
}

const ActionPanelRoot: React.FC<ActionPanel.Props> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements["action-panel"] = props;

	return <action-panel {...nativeProps} />;
};

const ActionPanelSection: React.FC<
	React.PropsWithChildren<ActionPanel.Section.Props>
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

const ActionPannelSubmenu: React.FC<ActionPanel.Submenu.Props> = ({
	children,
	icon,
	...props
}) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return (
		<action-panel-submenu {...props} icon={serializedIcon}>
			{children}
		</action-panel-submenu>
	);
};

/**
 * @category Actions
 */
export const ActionPanel = Object.assign(ActionPanelRoot, {
	Section: ActionPanelSection,
	Submenu: ActionPannelSubmenu,
});
