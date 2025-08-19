import * as React from 'react';
import type { ListItemDetailProps } from '../components/list';
import { SerializedImageLike } from '../image';
import { SerializedColorLike } from '../color';
import { Keyboard } from '../keyboard';
type HandlerId = string;
declare global {
    namespace JSX {
        interface IntrinsicElements {
            detail: {
                navigationTitle?: string;
                markdown: string;
            };
            list: {
                actions?: React.ReactNode;
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
                onOpen?: () => void;
                onSearchTextChange?: (text: string) => void;
                children?: React.ReactNode;
            };
            'action-panel-section': {
                title?: string;
                children?: React.ReactNode;
            };
            'action': {
                title: string;
                onAction: () => void;
                shortcut?: Keyboard.Shortcut;
                icon?: SerializedImageLike;
            };
            'tag-list': {
                title?: string;
                children?: React.ReactNode;
            };
            'tag-item': {
                color?: SerializedColorLike;
                icon?: SerializedImageLike;
                text?: string;
                onAction?: () => void;
            };
            'root-form': {
                enableDrafts: boolean;
                isLoading: boolean;
                navigationTitle?: string;
                children?: React.ReactNode;
            };
            'text-field': {};
            'password-field': {};
            'textarea': {};
        }
    }
}
export declare const Fragment: unique symbol;
export declare function jsxs(type: any, props: any, key: any): {
    $$typeof: symbol;
    type: any;
    key: any;
    props: any;
};
export declare function jsx(type: any, props: any, key: any): {
    $$typeof: symbol;
    type: any;
    key: any;
    props: any;
};
export {};
//# sourceMappingURL=jsx-runtime.d.ts.map