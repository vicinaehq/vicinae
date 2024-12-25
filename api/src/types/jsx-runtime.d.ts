import * as React from 'react';
import type { ListProps, ListItemProps, ListItemDetailProps, ListItemDetailMetadataProps, ListItemDetailMetadataLabelProps, ListItemDetailMetadataSeparator } from '../components/list';
import { ActionPanelProps } from '../components/action-pannel';
import { ActionProps, FinalizedActionProps } from '../components';
import { ImageLike, SerializedImageLike } from '../image';
import { SerializedColorLike } from '../color';

declare module 'react' {
	namespace JSX {
		interface IntrinsicElements {
			detail: {
				navigationTitle?: string;
				markdown: string;
			},
			list: {
				actions?: React.ReactNode;
				children?: React.ReactNode;
				filtering?: boolean;
				isLoading?: boolean;
				isShowingDetail?: boolean;
				searchBarPlaceholder?: string;
				navigationTitle?: string;
				onSearchTextChange?: string;
				onSelectionChange?: (id: string) => void;
			},
			'list-item': {
				title: string;
				id?: string;
				subtitle?: string;
				icon?: SerializedImageLike;
			},
			'list-item-detail': ListItemDetailProps,
			'list-item-detail-metadata': ListItemDetailMetadataProps

			metadata: {
				children?: ReactNode;
			},

			'metadata-label': {
				title: string;
				text: string;
				icon?: SerializedImageLike;
			}

			'metadata-separator': {},

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
