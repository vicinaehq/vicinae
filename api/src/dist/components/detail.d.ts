import { ReactNode } from 'react';
export type DetailProps = {
    navigationTitle?: string;
    metadata?: ReactNode;
    markdown: string;
};
export declare const Detail: import("react").FC<DetailProps> & {
    Metadata: import("react").FC<import("./metadata").MetadataProps> & {
        Label: import("react").FC<import("./metadata").ListItemDetailMetadataLabelProps>;
        Separator: import("react").FC<{}>;
        TagList: import("react").FC<import("./tag").TagListProps> & {
            Item: import("react").FC<import("./tag").TagItemProps>;
        };
    };
};
//# sourceMappingURL=detail.d.ts.map