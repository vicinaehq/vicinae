import React, { ReactNode, useEffect, useRef } from 'react';
import { ImageLike, serializeImageLike } from '../image';
import { TagList } from './tag';
import { bus } from '../bus';
import { randomUUID } from 'crypto';

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

export type ListItemDetailMetadataProps = {
	children: React.ReactNode;
};

export type ListItemDetailMetadataLabelProps = {
	title: string;
	text: string;
	icon?: string;
};


export type ListItemDetailMetadataSeparator = {
}

const ListRoot: React.FC<ListProps> = ({ onSearchTextChange, ...props }) => {
	const handlerId = useRef<string>(randomUUID());

	useEffect(() => {
		if (onSearchTextChange) {
			console.log(`mount handler ${handlerId.current}`);
			bus!.subscribe(handlerId.current, onSearchTextChange);
			
			return () => { bus!.unsubscribe(handlerId.current); }
		}
	}, []);

	return <list onSearchTextChange={onSearchTextChange ? handlerId.current : undefined} {...props} />;
}

const ListItem: React.FC<ListItemProps> = ({ detail, actions, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['list-item'] = {
		title: props.title,
		subtitle: props.subtitle,
		id: props.id,
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

const Metadata: React.FC<ListItemDetailMetadataProps> = (props) => {
	return <list-item-detail-metadata {...props} />;
}

const MetadataLabel: React.FC<ListItemDetailMetadataLabelProps> = (props) => {
	return <list-item-detail-metadata-label {...props} />
}

const MetadataSeparator: React.FC<{}> = (props) => {
	return <list-item-detail-metadata-separator {...props} />
}


export const List = Object.assign(ListRoot, {
	Item: Object.assign(ListItem, {
		Detail: Object.assign(ListItemDetail, {
			Metadata: Object.assign(Metadata, {
				Label: MetadataLabel,
				Separator: MetadataSeparator,
				TagList
			})
		})
	})
});
