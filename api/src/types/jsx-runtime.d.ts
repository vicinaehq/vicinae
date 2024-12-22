import * as React from 'react';
import type { ListProps, ListItemProps, ListItemDetailProps, ListItemDetailMetadataProps, ListItemDetailMetadataLabelProps, ListItemDetailMetadataSeparator } from '../components/list';
import { ActionPanelProps } from '../components/action-pannel';
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
			'action-panel': {
				title?: string;
				children?: ReactNode;
			},
			'action-panel-submenu': {
				title: string;
				icon?: SerializedImageLike;
				onOpen?: () => void;
				onSearchTextChange?: (text: string) => void;
				children?: React.ReactNode;
			},
			'action-panel-section': {
				title?: section;
				children?: ReactNode;
			},
			'action': {
				title: string;
				onAction: () => void,
				icon?: SerializedImageLike;
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
