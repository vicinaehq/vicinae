import React, { FC, ReactNode } from "react";
import { type ColorLike, serializeColorLike } from "../color";
import { type ImageLike, serializeProtoImage } from "../image";

export type TagListProps = {
	title: string;
	children: ReactNode;
};

const TagListRoot: FC<TagListProps> = ({ title, children }) => {
	const nativeProps: React.JSX.IntrinsicElements["tag-list"] = {
		title,
		children,
	};

	return <tag-list {...nativeProps} />;
};

export type TagItemProps = {
	color?: ColorLike;
	icon?: ImageLike;
	text?: string;
	onAction?: () => void;
};

const TagItem: FC<TagItemProps> = ({ color, icon, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements["tag-item"] = {
		...props,
		color: color ? serializeColorLike(color) : undefined,
		icon: icon ? serializeProtoImage(icon) : undefined,
	};

	return <tag-item {...nativeProps} />;
};

export const TagList = Object.assign(TagListRoot, {
	Item: TagItem,
});
