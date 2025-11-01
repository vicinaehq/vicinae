import React from "react";
import { ImageLike } from "../image";
import { TagList } from "./tag";

export type MetadataProps = {
	children?: React.ReactNode;
};

export type ListItemDetailMetadataLabelProps = {
	title: string;
	text: string;
	icon?: ImageLike;
};

export type ListItemDetailMetadataSeparator = {};

const MetadataRoot: React.FC<MetadataProps> = (props) => {
	return <metadata {...props} />;
};

const MetadataLabel: React.FC<ListItemDetailMetadataLabelProps> = (props) => {
	return <metadata-label {...props} />;
};

const MetadataSeparator: React.FC = () => {
	return <metadata-separator />;
};

const MetadataLink: React.FC<{
	title: string;
	target: string;
	text: string;
}> = ({ title, target, text }) => {
	return <metadata-link title={title} target={target} text={text} />;
};

export const Metadata = Object.assign(MetadataRoot, {
	Label: MetadataLabel,
	Separator: MetadataSeparator,
	TagList,
	Link: MetadataLink,
});
