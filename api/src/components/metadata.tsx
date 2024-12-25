import React from 'react';
import { ImageLike, serializeImageLike } from '../image';
import { TagList } from './tag';

export type MetadataProps = {
	children?: React.ReactNode;
};

export type ListItemDetailMetadataLabelProps = {
	title: string;
	text: string;
	icon?: ImageLike;
};


export type ListItemDetailMetadataSeparator = {
}

const MetadataRoot: React.FC<MetadataProps> = (props) => {
	return <metadata {...props} />;
}

const MetadataLabel: React.FC<ListItemDetailMetadataLabelProps> = ({ icon, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['metadata-label'] = props;

	if (icon) nativeProps.icon = serializeImageLike(icon);

	return <metadata-label {...props} />
}

const MetadataSeparator: React.FC = () => {
	return <metadata-separator />
}

export const Metadata = Object.assign(MetadataRoot, {
	Label: MetadataLabel,
	Separator: MetadataSeparator,
	TagList
});
