import React, { ReactNode } from "react";
import { ImageLike, serializeProtoImage } from "../image";

export type EmptyViewProps = {
	title?: string;
	icon?: ImageLike;
	description?: string;
	actions?: ReactNode;
};

export const EmptyView: React.FC<EmptyViewProps> = ({
	actions,
	icon,
	...props
}) => {
	const serializedIcon = icon ? serializeProtoImage(icon) : icon;
	return (
		<empty-view {...props} icon={serializedIcon}>
			{actions}
		</empty-view>
	);
};
