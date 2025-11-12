import { ReactNode } from "react";
import { type Image, serializeProtoImage } from "../image";

export declare namespace Dropdown {
	type Props = DropdownProps;
	type Item = DropdownItemProps;
	type Section = DropdownSectionProps;
}

type DropdownProps = {
	tooltip?: string;
	children?: ReactNode;
	defaultValue?: string;
	filtering?: boolean;
	id?: string;
	isLoading?: boolean;
	placeholder?: string;
	storeValue?: boolean;
	throttle?: boolean;
	value?: string;
	onChange?: (newValue: string) => void;
	onSearchTextChange?: (text: string) => void;
};

type DropdownItemProps = {
	title: string;
	value: string;
	icon?: Image.ImageLike;
	keywords?: string[];
};

type DropdownSectionProps = {
	title?: string;
	children?: ReactNode;
};

const DropdownRoot: React.FC<DropdownProps> = ({ children, ...props }) => {
	return <dropdown {...props}>{children}</dropdown>;
};

const Item: React.FC<DropdownItemProps> = ({ title, value, icon }) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return <dropdown-item title={title} value={value} icon={serializedIcon} />;
};

const Section: React.FC<DropdownSectionProps> = ({ title, children }) => {
	return <dropdown-section title={title}>{children}</dropdown-section>;
};

export const Dropdown = Object.assign(DropdownRoot, {
	Item,
	Section,
});
