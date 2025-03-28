import React, { ReactNode, useEffect, useRef } from 'react';
import { ImageLike, serializeImageLike } from '../image';
import { bus } from '../bus';
import { randomUUID } from 'crypto';
import { Metadata } from './metadata';
import { EmptyView } from './empty-view';
import { useEventListener } from '../hooks';

export type ListProps = {
	actions?: React.ReactNode;
	children?: React.ReactNode;
	filtering?: boolean;
	isLoading?: boolean;
	isShowingDetail?: boolean;
	searchBarPlaceholder?: string;
	navigationTitle?: string;
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
};

export type ListItemDetailProps = {
	isLoading?: boolean;
	markdown?: string;
	metadata?: React.ReactNode
};

const ListRoot: React.FC<ListProps> = ({ onSearchTextChange, onSelectionChange, ...props }) => {
	const searchTextChangeHandler = useEventListener(onSearchTextChange);
	const selectionChangeHandler = useEventListener(onSelectionChange);

	return <list 
		onSearchTextChange={searchTextChangeHandler} 
		onSelectionChange={selectionChangeHandler} 
		{...props}
	/>;
}

const ListItem: React.FC<ListItemProps> = ({ detail, actions, ...props }) => {
	const id = useRef(props.id ?? randomUUID());
	const nativeProps: React.JSX.IntrinsicElements['list-item'] = {
		title: props.title,
		subtitle: props.subtitle,
		id: id.current
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

export const List = Object.assign(ListRoot, {
	Section: ListSection,
	EmptyView,
	Item: Object.assign(ListItem, {
		Detail: Object.assign(ListItemDetail, {
			Metadata
		})
	})
});
