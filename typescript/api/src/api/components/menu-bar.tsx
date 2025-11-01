import { ReactNode } from "react";
import { Image } from "../image";
import { Keyboard } from "../keyboard";

type MenuBarExtraProps = {
	icon?: Image.ImageLike;
	isLoading?: boolean;
	title?: string;
	tooltip?: string;
	children?: ReactNode;
};

type MenuBarExtraItemProps = {
	title: string;
	alternate?: any;
	icon?: Image.ImageLike;
	shortcut?: Keyboard.Shortcut;
	subtitle?: string;
	tooltip?: string;
	onAction?: (event: any) => void;
};

const Root: React.FC<MenuBarExtraProps> = () => {
	return <menu-bar />;
};

const Item: React.FC<MenuBarExtraItemProps> = () => {
	return <menu-bar-item />;
};

const Submenu: React.FC<any> = () => {
	return <menu-bar-submenu />;
};

const Section: React.FC<any> = () => {
	return <menu-bar-section />;
};

const Separator: React.FC<any> = () => {
	return <separator />;
};

export const MenuBarExtra = Object.assign(Root, {
	Item,
	Submenu,
	Section,
	Separator,
});
