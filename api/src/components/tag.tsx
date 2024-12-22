import React, { FC, ReactNode } from 'react';
import { ImageLike, serializeImageLike } from '../image';
import { ColorLike, serializeColorLike } from '../color';

export type TagListProps = {
	title: string;
	children: ReactNode;
};

const TagListRoot: FC<TagListProps> = ({ title, children }) => {
	const nativeProps: React.JSX.IntrinsicElements['tag-list'] = {
		title,
		children
	};

	return <tag-list {...nativeProps} />
}

export type TagItemProps = {
	color?: ColorLike;
	icon?: ImageLike;
	text?: string;
	onAction?: () => void;
};

const TagItem: FC<TagItemProps> = ({ color, icon, text, onAction }) => {
	const nativeProps: React.JSX.IntrinsicElements['tag-item'] = {
		text, onAction
	};

	if (color) nativeProps.color = serializeColorLike(color);
	if (icon) nativeProps.icon = serializeImageLike(icon);

	return <tag-item {...nativeProps} />
}

export const TagList = Object.assign(TagListRoot, {
	Item: TagItem
});
