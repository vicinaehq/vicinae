import React, { ReactNode } from 'react';
import { ImageLike } from '../image';
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
    metadata?: React.ReactNode;
};
type ListSectionProps = {
    title?: string;
    subtitle?: string;
    children?: ReactNode;
};
export declare const List: React.FC<ListProps> & {
    Section: React.FC<ListSectionProps>;
    EmptyView: React.FC<import("./empty-view").EmptyViewProps>;
    Item: React.FC<ListItemProps> & {
        Detail: React.FC<ListItemDetailProps> & {
            Metadata: React.FC<import("./metadata").MetadataProps> & {
                Label: React.FC<import("./metadata").ListItemDetailMetadataLabelProps>;
                Separator: React.FC<{}>;
                TagList: React.FC<import("./tag").TagListProps> & {
                    Item: React.FC<import("./tag").TagItemProps>;
                };
            };
        };
    };
};
export {};
//# sourceMappingURL=list.d.ts.map