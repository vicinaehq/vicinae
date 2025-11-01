import React, { ReactNode } from "react";
import { ImageLike } from "../image";

export type EmptyViewProps = {
	title?: string;
	icon?: ImageLike;
	description?: string;
	actions?: ReactNode;
};

export const EmptyView: React.FC<EmptyViewProps> = ({ actions, ...props }) => {
	return <empty-view {...props}>{actions}</empty-view>;
};
