import React, { ReactNode, useRef } from "react";
import { type Image, type ImageLike, serializeProtoImage } from "../image";
import { randomUUID } from "crypto";
import { EmptyView } from "./empty-view";
import { type ColorLike, serializeColorLike } from "../color";
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

/**
 * Controls how the grid content should fit its allocated space.
 */
enum GridFit {
	/**
	 * The content will be contained within the grid cell, with vertical/horizontal bars if its aspect ratio differs from the cell's
	 */
	Contain = "contain",
	/**
	 * The content will be scaled proportionally so that it fill the entire cell; parts of the content could end up being cropped out.
	 */
	Fill = "fill",
}

/**
 * A grid component is very similar to the {@link List} component, but is optimized to render items
 * that are primarily represented as an icon or an image.
 *
 * Grids are commonly used to implement wallpaper, icon or emoji pickers.
 *
 * Grid can be organized in sections and define an arbitrary number of columns and aspect ratio.
 *
 * Grid can also render cells filled with a single color, which can be useful in order to implement color pickers.
 *
 * ![](../../../assets/grid.png)
 *
 * @category UI Components
 */
export namespace Grid {
	type BaseSection = {
		/**
		 * Amount of space to keep free around the edges of each item.
		 * The real amount of space this translates to depends on the selected on the aspect ratio
		 * and the number of columns.
		 */
		inset?: GridInset;
		itemSize?: GridItemSize;

		/**
		 * How many items to fit in a single row.
		 */
		columns?: number;

		fit?: GridFit;

		/**
		 * Specific aspect ratio to enforce for every cell.
		 */
		aspectRatio?: Grid.AspectRatio;
	};

	export type Props = BaseSection & {
		/**
		 * Amount of space to keep free around the edges of each item.
		 * The real amount of space this translates to depends on the selected on the aspect ratio
		 * and the number of columns.
		 */
		inset?: GridInset;
		itemSize?: GridItemSize;
		columns?: number;
		fit?: GridFit;
		aspectRatio?: Grid.AspectRatio;

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

	/**
	 * A specific subsection of the grid.
	 * Each subsection can specificy its own number of columns and aspect ratio.
	 */
	export namespace Section {
		export type Props = BaseSection & {
			title?: string;
			subtitle?: string;
			/**
			 * The grid items that are part of this section.
			 * @see {@link Grid.Item}
			 */
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
			| { value: Image.ImageLike | { color: ColorLike }; tooltip?: string };
			id?: string;
			subtitle?: string;
			actions?: ReactNode;
			accessory?: Grid.Item.Accessory;
		};

		export type Accessory = {
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

const GridItem: React.FC<Grid.Item.Props> = ({
	detail,
	actions,
	content,
	accessory,
	...props
}) => {
	const id = useRef(props.id ?? randomUUID());

	// Remove value wrapper
	const innerContent =
		typeof content === "object" && "value" in content ? content.value : content;

	const tooltip =
		typeof content === "object" && "tooltip" in content
			? content.tooltip
			: undefined;

	// Content
	let serializedContent: React.JSX.IntrinsicElements["grid-item"]["content"];
	if (
		innerContent &&
		typeof innerContent === "object" &&
		"color" in innerContent
	) {
		serializedContent = {
			color: serializeColorLike(innerContent.color),
		};
	} else {
		serializedContent = innerContent
			? serializeProtoImage(innerContent)
			: undefined;
	}

	// Accessory
	const serializedAccessory = accessory
		? {
			icon: accessory.icon
				? serializeProtoImage(accessory.icon)
				: accessory.icon,
			tooltip: accessory.tooltip,
		}
		: undefined;

	return (
		<grid-item
			{...props}
			content={serializedContent}
			tooltip={tooltip}
			accessory={serializedAccessory}
			id={id.current}
		>
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

/**
 * @category UI Components
 */
export const Grid = Object.assign(GridRoot, {
	Section: GridSection,
	EmptyView,
	Dropdown,
	Fit: GridFit,
	Inset: GridInset,
	ItemSize: GridItemSize,
	Item: Object.assign(GridItem, {}),
});
