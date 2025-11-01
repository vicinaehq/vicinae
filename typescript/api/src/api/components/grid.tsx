import React, { ReactNode, useRef } from "react";
import { Image, ImageLike } from "../image";
import { randomUUID } from "crypto";
import { EmptyView } from "./empty-view";
import { Color, ColorLike } from "../color";
import { Dropdown } from "./dropdown";

enum GridInset {
	Zero = "zero",
	Small = "small",
	Medium = "medium",
	Large = "large",
}

/**
 * Enum representing the number of items that should be displayed on a single row.
 * @deprecated - use `columns` instead.
 */
enum GridItemSize {
	Small = "small", // Fits 8 items per row.
	Medium = "medium", // Fits 5 items per row.
	Large = "large", // Fits 3 items per row.
}

const aspectRatioMap: Record<Grid.AspectRatio, number> = {
	"1": 1,
	"3/2": 3 / 2,
	"2/3": 2 / 3,
	"4/3": 4 / 3,
	"3/4": 3 / 4,
	"16/9": 16 / 9,
	"9/16": 9 / 16,
	"21/9": 21 / 9,
	"9/21": 9 / 21,
	"32/9": 32 / 9,
	"9/32": 9 / 32,
};

enum GridFit {
	Contain = "contain",
	Fill = "fill",
}

export namespace Grid {
	type BaseSection = {
		inset?: GridInset;
		itemSize?: GridItemSize;
		columns?: number;
		fit?: GridFit;
		aspectRatio?: Grid.AspectRatio;
	};

	export type Props = BaseSection & {
		actions?: React.ReactNode;
		children?: React.ReactNode;
		filtering?: boolean;
		/**
		 * @deprecated use filtering
		 */
		enableFiltering?: boolean;
		isLoading?: boolean;
		searchText?: string;
		searchBarPlaceholder?: string;
		navigationTitle?: string;
		searchBarAccessory?: ReactNode;
		onSearchTextChange?: (text: string) => void;
		onSelectionChange?: (id: string) => void;
	};

	export namespace Section {
		export type Props = BaseSection & {
			title?: string;
			subtitle?: string;
			children?: ReactNode;
		};
	}

	export type Fit = GridFit;
	export type Inset = GridInset;
	export type ItemSize = GridItemSize;
	export type AspectRatio =
		| "1"
		| "3/2"
		| "2/3"
		| "4/3"
		| "3/4"
		| "16/9"
		| "9/16"
		| "21/9"
		| "9/21"
		| "32/9"
		| "9/32";

	export namespace Item {
		export type Props = {
			title?: string;
			detail?: React.ReactNode;
			keywords?: string[];
			icon?: ImageLike;
			content:
				| Image.ImageLike
				| { color: ColorLike }
				| { value: Image.ImageLike; tooltip?: string };
			id?: string;
			subtitle?: string;
			actions?: ReactNode;
			accessories?: Grid.Item.Accessory[];
		};

		type Tag =
			| string
			| Date
			| undefined
			| null
			| { color: ColorLike; value: string | Date | undefined | null };
		type Text =
			| string
			| Date
			| undefined
			| null
			| { color: Color; value: string | Date | undefined | null };

		export type Accessory = ({ tag?: Tag } | { text?: Text }) & {
			icon?: Image.ImageLike;
			tooltip?: string | null;
		};
	}
}

const GridRoot: React.FC<Grid.Props> = ({
	searchBarAccessory,
	children,
	actions,
	inset,
	itemSize,
	fit = GridFit.Contain,
	aspectRatio = "1",
	...props
}) => {
	if (
		typeof props.enableFiltering === "boolean" &&
		typeof props.filtering === "undefined"
	) {
		props.filtering = props.enableFiltering;
	}

	if (props.columns === undefined && itemSize) {
		props.columns = {
			[GridItemSize.Small]: 8,
			[GridItemSize.Medium]: 5,
			[GridItemSize.Large]: 3,
		}[itemSize];
	}

	return (
		<grid
			fit={fit}
			inset={inset}
			aspectRatio={aspectRatioMap[aspectRatio]}
			{...props}
		>
			{searchBarAccessory}
			{children}
			{actions}
		</grid>
	);
};

const GridItem: React.FC<Grid.Item.Props> = ({ detail, actions, ...props }) => {
	const id = useRef(props.id ?? randomUUID());

	return (
		<grid-item {...props} id={id.current}>
			{detail}
			{actions}
		</grid-item>
	);
};

const GridSection: React.FC<Grid.Section.Props> = ({
	fit,
	aspectRatio,
	inset,
	...props
}) => {
	const nativeProps: React.JSX.IntrinsicElements["grid-section"] = {
		fit,
		aspectRatio: aspectRatio ? aspectRatioMap[aspectRatio] : aspectRatio,
		inset,
		...props,
	};

	return <grid-section {...nativeProps} />;
};

export const Grid = Object.assign(GridRoot, {
	Section: GridSection,
	EmptyView,
	Dropdown,
	Fit: GridFit,
	Inset: GridInset,
	ItemSize: GridItemSize,
	Item: Object.assign(GridItem, {}),
});
