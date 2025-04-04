import * as React from 'react';
import type { ListItemDetailProps } from '../components/list';
import { SerializedImageLike } from '../image';
import { SerializedColorLike } from '../color';
import { Keyboard } from '../keyboard';
type HandlerId = string;

import 'react';

declare module 'react' {
	namespace JSX {
	interface IntrinsicElements {
            detail: {
                navigationTitle?: string;
                markdown: string;
            };
            list: {
                children?: React.ReactNode;
                filtering?: boolean;
                isLoading?: boolean;
                isShowingDetail?: boolean;
                searchBarPlaceholder?: string;
                navigationTitle?: string;
                onSearchTextChange?: HandlerId;
                onSelectionChange?: HandlerId;
            };
            'list-section': {
                title?: string;
                subtitle?: string;
                children?: React.ReactNode;
            };
            'list-item': {
                title: string;
                id?: string;
                subtitle?: string;
                icon?: SerializedImageLike;
            };
            'list-item-detail': ListItemDetailProps;
            'list-item-detail-metadata': any;
			'list-accessory': {
			}
            'empty-view': {
                description?: string;
                title?: string;
                icon?: SerializedImageLike;
            };
            metadata: {
                children?: React.ReactNode;
            };
            'metadata-label': {
                title: string;
                text: string;
                icon?: SerializedImageLike;
            };
            'metadata-separator': {};
            'action-panel': {
                title?: string;
                children?: React.ReactNode;
            };
            'action-panel-submenu': {
                title: string;
                icon?: SerializedImageLike;
                onOpen?: HandlerId;
                onSearchTextChange?: HandlerId;
                children?: React.ReactNode;
            };
            'action-panel-section': {
                title?: string;
                children?: React.ReactNode;
            };
            'action': {
                title: string;
                onAction: HandlerId;
                shortcut?: Keyboard.Shortcut;
                icon?: SerializedImageLike;
				autoFocus?: boolean;
            };
            'tag-list': {
                title?: string;
                children?: React.ReactNode;
            };
            'tag-item': {
                color?: SerializedColorLike;
                icon?: SerializedImageLike;
                text?: string;
                onAction: HandlerId;
            };
            'root-form': {
                enableDrafts: boolean;
                isLoading: boolean;
                navigationTitle?: string;
                children?: React.ReactNode;
            };
            'text-field': {};
			'date-picker': {},
            'password-field': {};
            'textarea': {};

			'dropdown': {},
			'dropdown-section': {
				title?: string;
				children: ReactNode;
			},
			'dropdown-item': {
				title: string;
				value: string;
				icon?: SerializedImageLike;
				keywords?: string[];
			},

			'separator': {},
			'menu-bar': {},
			'menu-bar-item': {},
			'menu-bar-submenu': {},
			'menu-bar-section': {},
        }
	}
}
