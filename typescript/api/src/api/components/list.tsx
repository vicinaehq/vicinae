import React, { ReactNode, useRef } from "react";
import { Image, ImageLike } from "../image";
import { randomUUID } from "crypto";
import { Metadata } from "./metadata";
import { EmptyView } from "./empty-view";
import { Color, ColorLike } from "../color";
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
			icon?: ImageLike;
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

		type AccessoryBase = string | Date | undefined | null;

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

const ListItem: React.FC<List.Item.Props> = ({ detail, actions, ...props }) => {
	const id = useRef(props.id ?? randomUUID());

	return (
		<list-item {...props} id={id.current}>
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
