import React from 'react';
import { ImageLike, serializeImageLike } from '../image';

export type EmptyViewProps = {
	title?: string;
	icon?: ImageLike;
	description?: string;
};

export const EmptyView: React.FC<EmptyViewProps> = ({ icon, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['empty-view'] = props;

	if (icon) nativeProps.icon = serializeImageLike(icon);

	return <empty-view {...nativeProps} />
}
