import React, { ReactNode } from 'react';
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
    icon?: string;
    id?: string;
    subtitle?: string;
    actions?: ReactNode;
};
export type ListItemDetailProps = {
    isLoading?: boolean;
    markdown?: string;
    metadata?: React.ReactNode;
};
export type ListItemDetailMetadataProps = {
    children: React.ReactNode;
};
export type ListItemDetailMetadataLabelProps = {
    title: string;
    text: string;
    icon?: string;
};
export type ListItemDetailMetadataSeparator = {};
export declare const List: React.FC<ListProps> & {
    Item: React.FC<ListItemProps> & {
        Detail: React.FC<ListItemDetailProps> & {
            Metadata: React.FC<ListItemDetailMetadataProps> & {
                Label: React.FC<ListItemDetailMetadataLabelProps>;
                Separator: React.FC<{}>;
            };
        };
    };
};
//# sourceMappingURL=list.d.ts.map