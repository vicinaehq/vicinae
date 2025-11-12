import React, { ReactNode, useRef } from "react";
import {
	type Image,
	type ImageLike,
	type SerializedImageLike,
	serializeProtoImage,
} from "../image";
import { randomUUID } from "crypto";
import { Metadata } from "./metadata";
import { EmptyView } from "./empty-view";
import {
	type Color,
	type ColorLike,
	type SerializedColorLike,
	serializeColorLike,
} from "../color";
import { Dropdown } from "./dropdown";

export declare namespace List {
	export type Props = {
		actions?: React.ReactNode;
		children?: React.ReactNode;
		filtering?: boolean;
		/**
		 * @deprecated use filtering
		 */
		enableFiltering?: boolean;
		isLoading?: boolean;
		isShowingDetail?: boolean;
		searchText?: string;
		searchBarPlaceholder?: string;
		navigationTitle?: string;
		searchBarAccessory?: ReactNode;
		throttle?: boolean;
		onSearchTextChange?: (text: string) => void;
		onSelectionChange?: (id: string) => void;
	};

	export namespace Section {
		export type Props = {
			title?: string;
			subtitle?: string;
			children?: ReactNode;
		};
	}

	export namespace Item {
		export type Props = {
			title: string;
			keywords?: string[];
			detail?: React.ReactNode;
			icon?:
				| ImageLike
				| {
						value: ImageLike | undefined | null;
						tooltip: string;
				  };
			id?: string;
			subtitle?: string;
			actions?: ReactNode;
			accessories?: List.Item.Accessory[];
		};

		export namespace Detail {
			export type Props = {
				isLoading?: boolean;
				markdown?: string;
				metadata?: React.ReactNode;
			};
		}

		export type AccessoryBase = string | Date | undefined | null;

		type Tag =
			| AccessoryBase
			| { color: ColorLike; value: string | Date | undefined | null };
		type Text =
			| AccessoryBase
			| { color: Color; value: string | Date | undefined | null };

		export type Accessory = ({ tag?: Tag } | { text?: Text }) & {
			icon?: Image.ImageLike;
			tooltip?: string | null;
		};
	}
}

// used in jsx.d.ts, not for public api
export type SerializedTag =
	| List.Item.AccessoryBase
	| { color: SerializedColorLike; value: string | Date | undefined | null };
export type SerializedText =
	| List.Item.AccessoryBase
	| { color: SerializedColorLike; value: string | Date | undefined | null };
export type SerializedAccessory = (
	| { tag?: SerializedTag }
	| { text?: SerializedText }
) & {
	icon?: SerializedImageLike;
	tooltip?: string | null;
};

function serializeAccessory(
	accessory: List.Item.Accessory,
): SerializedAccessory {
	const icon = accessory.icon ? serializeProtoImage(accessory.icon) : undefined;

	const tag =
		typeof accessory === "object" && "tag" in accessory
			? serializeTag(accessory.tag)
			: undefined;
	const text =
		typeof accessory === "object" && "text" in accessory
			? serializeText(accessory.text)
			: undefined;

	return { icon, tooltip: accessory.tooltip, tag, text };
}

function serializeTag(tag: List.Item.Tag): SerializedTag {
	if (tag == null) return tag; // null or undefined
	if (typeof tag !== "object") return tag;

	if ("color" in tag) {
		const color = serializeColorLike(tag.color);
		const value = "value" in tag ? tag.value : undefined;
		return { color, value };
	}
}

function serializeText(text: List.Item.Text): SerializedText {
	if (text == null) return text; // null or undefined
	if (typeof text !== "object") return text;

	if ("color" in text) {
		const color = serializeColorLike(text.color);
		const value = "value" in text ? text.value : undefined;
		return { color, value };
	}
}

const ListRoot: React.FC<List.Props> = ({
	searchBarAccessory,
	children,
	actions,
	...props
}) => {
	if (
		typeof props.enableFiltering === "boolean" &&
		typeof props.filtering === "undefined"
	) {
		props.filtering = props.enableFiltering;
	}

	return (
		<list {...props}>
			{searchBarAccessory}
			{children}
			{actions}
		</list>
	);
};

const ListItem: React.FC<List.Item.Props> = ({
	detail,
	actions,
	icon,
	accessories,
	...props
}) => {
	const id = useRef(props.id ?? randomUUID());

	// Icon
	let serializedIcon: React.JSX.IntrinsicElements["list-item"]["icon"];
	if (icon && typeof icon === "object" && "value" in icon) {
		serializedIcon = {
			value: icon.value ? serializeProtoImage(icon.value) : undefined,
			tooltip: icon.tooltip,
		};
	} else {
		serializedIcon = icon ? serializeProtoImage(icon) : undefined;
	}

	// Accessories
	const serializedAccessories =
		accessories?.map(serializeAccessory) ?? undefined;

	return (
		<list-item
			{...props}
			icon={serializedIcon}
			accessories={serializedAccessories}
			id={id.current}
		>
			{detail}
			{actions}
		</list-item>
	);
};

const ListItemDetail: React.FC<List.Item.Detail.Props> = ({
	metadata,
	...props
}) => {
	return <list-item-detail {...props}>{metadata}</list-item-detail>;
};

const ListSection: React.FC<List.Section.Props> = (props) => {
	return <list-section {...props} />;
};

export const List = Object.assign(ListRoot, {
	Section: ListSection,
	EmptyView,
	Dropdown,
	Item: Object.assign(ListItem, {
		Detail: Object.assign(ListItemDetail, {
			Metadata,
		}),
	}),
});
