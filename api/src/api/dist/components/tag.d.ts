import React, { ReactNode } from 'react';
import { ImageLike } from '../image';
import { ColorLike } from '../color';
export type TagListProps = {
    title: string;
    children: ReactNode;
};
export type TagItemProps = {
    color?: ColorLike;
    icon?: ImageLike;
    text?: string;
    onAction?: () => void;
};
export declare const TagList: React.FC<TagListProps> & {
    Item: React.FC<TagItemProps>;
};
//# sourceMappingURL=tag.d.ts.map