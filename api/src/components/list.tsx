import React, { ReactNode, useRef } from 'react';
import { Image, ImageLike, serializeImageLike } from '../image';
import { randomUUID } from 'crypto';
import { Metadata } from './metadata';
import { EmptyView } from './empty-view';
import { useEventListener } from '../hooks';
import { Color, ColorLike } from '../color';
import { Dropdown } from './dropdown';

export declare namespace List {
	export namespace Item {
		export type Props = ListItemProps;

		type Tag = string | Date | undefined | null | { color: ColorLike, value: string | Date | undefined | null };
		type Text = string | Date | undefined | null | { color: Color, value: string | Date | undefined | null };

		export type Accessory  = ({ tag?: Tag } | { text?: Text }) & {
				icon?: Image.ImageLike;
				tooltip?: string | null;
		};
	}
};

export type ListProps = {
	actions?: React.ReactNode;
	children?: React.ReactNode;
	filtering?: boolean;
	/**
	 * @deprecated use filtering
	 */
	enableFiltering?: boolean;
	isLoading?: boolean;
	isShowingDetail?: boolean;
	searchText?: string;
	searchBarPlaceholder?: string;
	navigationTitle?: string;
	searchBarAccessory?: ReactNode;
	onSearchTextChange?: (text: string) => void;
	onSelectionChange?: (id: string) => void;
};

export type ListItemProps = {
	title: string;
	detail?: React.ReactNode;
	icon?: ImageLike;
	id?: string;
	subtitle?: string;
	actions?: ReactNode;
	accessories?: List.Item.Accessory[];
};

export type ListItemDetailProps = {
	isLoading?: boolean;
	markdown?: string;
	metadata?: React.ReactNode
};

const ListRoot: React.FC<ListProps> = ({ onSearchTextChange, searchBarAccessory, onSelectionChange, children, actions, ...props }) => {
	const searchTextChangeHandler = useEventListener(onSearchTextChange);
	const selectionChangeHandler = useEventListener(onSelectionChange);

	if (typeof props.enableFiltering === "boolean" && typeof props.filtering === "undefined") {
		props.filtering = props.enableFiltering;
	}

	return <list 
		onSearchTextChange={searchTextChangeHandler} 
		onSelectionChange={selectionChangeHandler} 
		{...props}
	>
		{searchBarAccessory}
		{children}
		{actions}
	</list>
}

const ListItem: React.FC<ListItemProps> = ({ detail, actions, ...props }) => {
	const id = useRef(props.id ?? randomUUID());
	const nativeProps: React.JSX.IntrinsicElements['list-item'] = {
		title: props.title,
		subtitle: props.subtitle,
		id: id.current,
	};

	if (props.icon) nativeProps.icon = serializeImageLike(props.icon);

	return (
		<list-item {...nativeProps}>
			{detail}
			{actions}
		</list-item>
	);
}

const ListItemDetail: React.FC<ListItemDetailProps> = ({ metadata, ...props }) => {
	return (
		<list-item-detail {...props}>
			{metadata}
		</list-item-detail>
	)
}

type ListSectionProps = {
	title?: string;
	subtitle?: string;
	children?: ReactNode;
};

const ListSection: React.FC<ListSectionProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['list-section'] = props;

	return <list-section {...nativeProps} />
}

export const ListAccessory: React.FC<List.Item.Accessory> = (props) => {
	return <list-accessory />
}

export const List = Object.assign(ListRoot, {
	Section: ListSection,
	EmptyView,
	Dropdown,
	Item: Object.assign(ListItem, {
		Detail: Object.assign(ListItemDetail, {
			Metadata
		}),
		Accessory: ListAccessory
	}),
});
