import React from "react";
import { type ImageLike, serializeProtoImage } from "../image";
import { type ColorLike, serializeColorLike, type SerializedColorLike } from "../color";
import { TagList } from "./tag";

export type MetadataProps = {
	children?: React.ReactNode;
};

export type ListItemDetailMetadataLabelProps = {
	title: string;
	text: string | { color?: ColorLike; value: string };
	icon?: ImageLike;
};

export type ListItemDetailMetadataSeparator = {};

const MetadataRoot: React.FC<MetadataProps> = (props) => {
	return <metadata {...props} />;
};

const MetadataLabel: React.FC<ListItemDetailMetadataLabelProps> = (props) => {
	const serializedIcon = props.icon
		? serializeProtoImage(props.icon)
		: props.icon;

	let serializedText: string | { color?: SerializedColorLike; value: string };
	if (props.text && typeof props.text === "object") {
		serializedText = {
			color: props.text.color ? serializeColorLike(props.text.color) : undefined,
			value: props.text.value,
		};
	} else {
		serializedText = props.text;
	}

	const { text, ...restProps } = props;

	return <metadata-label {...restProps} icon={serializedIcon} text={serializedText} />;
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
