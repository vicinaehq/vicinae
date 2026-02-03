import { randomUUID } from "node:crypto";
import React, { type ReactNode, useRef } from "react";
import { type Image, serializeProtoImage } from "../image";
import type { Keyboard } from "../keyboard";

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
			//autoFocus?: boolean;
			//filtering?: boolean | { keepSectionOrder: boolean };
			//isLoading?: boolean;
			//throttle?: boolean;
			onOpen?: () => void;
			//onSearchTextChange?: (text: string) => void;
			children: ReactNode;
		};
	}
}

const ActionPanelRoot: React.FC<ActionPanel.Props> = (props) => {
	const stableIdRef = useRef<string | undefined>(undefined);
	if (!stableIdRef.current) {
		stableIdRef.current = randomUUID();
	}

	const nativeProps: React.JSX.IntrinsicElements["action-panel"] = {
		...props,
		stableId: stableIdRef.current,
	};

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
	const stableIdRef = useRef<string | undefined>(undefined);
	if (!stableIdRef.current) {
		stableIdRef.current = randomUUID();
	}

	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return (
		<action-panel-submenu
			{...props}
			icon={serializedIcon}
			stableId={stableIdRef.current}
		>
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
