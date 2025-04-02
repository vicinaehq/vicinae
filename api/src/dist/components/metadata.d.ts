import React from 'react';
import { ImageLike } from '../image';
export type MetadataProps = {
    children?: React.ReactNode;
};
export type ListItemDetailMetadataLabelProps = {
    title: string;
    text: string;
    icon?: ImageLike;
};
export type ListItemDetailMetadataSeparator = {};
export declare const Metadata: React.FC<MetadataProps> & {
    Label: React.FC<ListItemDetailMetadataLabelProps>;
    Separator: React.FC<{}>;
    TagList: React.FC<import("./tag").TagListProps> & {
        Item: React.FC<import("./tag").TagItemProps>;
    };
};
//# sourceMappingURL=metadata.d.ts.map