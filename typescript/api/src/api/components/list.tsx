import type { ReactNode } from "react";
import { useRef } from "react";
import {
	type Image,
	type ImageLike,
	type SerializedImageLike,
	serializeProtoImage,
} from "../image";
import { randomUUID } from "node:crypto";
import { Metadata } from "./metadata";
import { EmptyView } from "./empty-view";
import {
	type Color,
	type ColorLike,
	type SerializedColorLike,
	serializeColorLike,
} from "../color";
import { Dropdown } from "./dropdown";

/**
 * A List component that can be used to render a list of items sharing a similar representation.
 *
 * This component comes with many quality of life features out of the box:
 *
 * - Items are fuzzy searched by default, without the need for explicit filtering logic
 * - Items can be grouped in sections for clearer organization
 * - Search can be throttled in order to provide typeahead experiences, ideal when dealing with remote data sources
 * - Builtin loading indicator to show that something is loading
 * - For a specific item, can render additonal markdown and metadata in a side panel
 *
 * ![](../../../assets/list.png)
 *
 * @category UI Components
 */
export declare namespace List {
	export type Props = {
		/**
		 * Action panel to use when no list item matches the current search query.
		 */
		actions?: React.ReactNode;

		/**
		 * List items or sections to render inside this list.
		 */
		children?: React.ReactNode;

		/**
		 * Whether to use Vicinae's builtin fuzzy filtering.
		 *
		 * This is turned on by default unless a `onSearchTextChange` handler is passed, indicating that
		 * custom filtering is desired.
		 */
		filtering?: boolean;

		/**
		 * @deprecated use filtering
		 */
		enableFiltering?: boolean;

		/**
		 * If true, a loading indicator is shown right below the search bar to indicate loading activity.
		 */
		isLoading?: boolean;

		/**
		 * Whether to show the current list item's detail, if any.
		 * If the current list item does not have a `detail`, this does nothing.
		 */
		isShowingDetail?: boolean;

		/**
		 * Bind this prop to the value of the search text. This is used to turn the List into a [controlled component](https://react.dev/learn/sharing-state-between-components#controlled-and-uncontrolled-components).
		 */
		searchText?: string;

		/**
		 * The placeholder to show in the search bar if there is no search query.
		 */
		searchBarPlaceholder?: string;

		/**
		 * The navigation title to display on the bottom left of the status bar, next to the command icon.
		 * This defaults to the name of the command.
		 */
		navigationTitle?: string;

		/**
		 * Accessory to show on the right of the search input.
		 * The list component only supports rendering a dropdown, in order to provide more filtering options.
		 *
		 * @see Dropdown
		 */
		searchBarAccessory?: ReactNode;

		/**
		 * Throttle the search so that search text change events are sent after a short delay instead of instaneously.
		 * **Highly** recommended if you need to fetch data on every change.
		 * @default false
		 */
		throttle?: boolean;

		/**
		 * Called every time the user modifies the search text by typing or editing.
		 * This can be throttled using the `throttle` prop.
		 *
		 * @param text - The new search text
		 */
		onSearchTextChange?: (text: string) => void;

		/**
		 * Called every time the currently selected item changes.
		 * Note that this does *not* fire when transitioning from having a selected item to none at all.
		 */
		onSelectionChange?: (id: string) => void;
	};

	/**
	 * List items can be organized inside sections, in order to further categorize them.
	 *
	 * Note that item inside sections still benefit from automatic fuzzy filtering, but items remain grouped by
	 * section no matter their matching scores.
	 *
	 * A list can render sections and render items that are not inside any section.
	 * While this is generally considered bad design, it will work as list items placed outside sections
	 * will automatically be attached to an unnamed section.
	 *
	 * @example
	 * ```typescript
	 * export default MyCommand() {
	 * return (
	 *   <List>
	 *     <List.Section title="Section 1">
	 * 	     <List.Item title="Item 1" />
	 * 		   <List.Item title="Item 2" />
	 *     </List.Section />
	 *     <List.Section title="Section 2">
	 * 	     <List.Item title="Item 1" />
	 * 	     <List.Item title="Item 2" />
	 * 	   </List.Section />
	 *   </List>
	 * 	);
	 * }
	 * ```
	 */
	export namespace Section {
		export type Props = {
			title?: string;
			subtitle?: string;
			children?: ReactNode;
		};
	}

	/**
	 * An individual list item rendered in a fixed size row.
	 */
	export namespace Item {
		export type Props = {
			title: string;

			/**
			 * Additional keywords the builtin filtering will consider when ranking items.
			 *
			 * Note that keywords match with a lower score than the title or subtitle fields.
			 *
			 * If builtin filtering is disabled, these are not used.
			 */
			keywords?: string[];

			/**
			 * Icon to show to the left of the item.
			 * @see {@link ImageLike}
			 */
			icon?:
			| ImageLike
			| {
				value: ImageLike | undefined | null;
				tooltip: string;
			};

			/**
			 * Unique identifier for this item.
			 * If not explicitly specified, Vicinae will create one automatically.
			 */
			id?: string;

			/**
			 * Subtitle to show next to the title, using a dampened color.
			 */
			subtitle?: string;

			/**
			 * Action panel to show when this item is selected.
			 */
			actions?: ReactNode;

			accessories?: List.Item.Accessory[];

			/**
			 * Additional information to display in a side panel if it is the currently selected item.
			 * @see {@link List.Item.Detail}
			 */
			detail?: React.ReactNode;
		};

		/**
		 * Side panel which can be used to render markdown text and an optional metadata section.
		 * In order for a detail to be shown, the List's `isShowingDetail` prop should be set to `true` and the
		 * currently selected item should have a valid `detail` prop.
		 *
		 * ![](../../../assets/list-detail.png)
		 */
		export namespace Detail {
			export type Props = {
				/**
				 * Whether to show a loading indicator under the search bar.
				 */
				isLoading?: boolean;

				/**
				 * Markdown content to render in the main view.
				 * @see {@link Detail}
				 */
				markdown?: string;

				/**
				 * Additional metadata.
				 * @see Metadata
				 */
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

/**
 * @category UI Components
 */
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
