import * as React from 'react';
import type { ListProps, ListItemProps, ListItemDetailProps, ListItemDetailMetadataProps, ListItemDetailMetadataLabelProps, ListItemDetailMetadataSeparator } from '../components/list';
import { ActionPannelProps } from '../components/action-pannel';
import { ActionProps, FinalizedActionProps } from '../components';
import { ImageLike, SerializedImageLike } from '../image';
import { SerializedColorLike } from '../color';

declare module 'react' {
	namespace JSX {
		interface IntrinsicElements {
			list: ListProps,
			'list-item': {
				title: string;
				id?: string;
				subtitle?: string;
				icon?: SerializedImageLike;
			},
			'list-item-detail': ListItemDetailProps,
			'list-item-detail-metadata': ListItemDetailMetadataProps
			'list-item-detail-metadata-label': ListItemDetailMetadataLabelProps,
			'list-item-detail-metadata-separator': ListItemDetailMetadataSeparator,
			'action-pannel': ActionPannelProps,
			'action': {
				onAction: () => void,
				icon?: ImageLike
			},
			'tag-list': {
				title?: string;
				children?: ReactNode;
			},
			'tag-item': {
				color?: SerializedColorLike,
				icon?: SerializedImageLike,
				text?: string;
				onAction?: () => void;
			}
		}
	}
}
