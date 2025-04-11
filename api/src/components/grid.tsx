import React, { ReactNode, useRef } from 'react';
import { Image, ImageLike, serializeImageLike } from '../image';
import { randomUUID } from 'crypto';
import { EmptyView } from './empty-view';
import { useEventListener } from '../hooks';
import { Color, ColorLike } from '../color';
import { Dropdown } from './dropdown';

export declare namespace Grid {
	enum Fit {
		Contain = 'contain',
		Fill = 'fill'
	}

	export type AspectRatio = '1' | '3/2' | '2/3' | '4/3' | '3/4' | '16/9' | '9/16';

	enum Inset {
		Small = 'small',
		Medium = 'medium',
		Large = 'large',
	}

	export namespace Section {
		type SectionConfig = {
			inset?: Inset;
			columns?: number;
			fit?: Grid.Fit;
			aspectRatio?: AspectRatio;
		}

		export type Props = SectionConfig & {
			title?: string;
			subtitle?: string;
			children?: ReactNode;
		};
	}

	export type Props = Grid.Section.SectionConfig & {
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

	export namespace Item {
		export type Props = {
			title: string;
			detail?: React.ReactNode;
			icon?: ImageLike;
			id?: string;
			subtitle?: string;
			actions?: ReactNode;
			accessories?: Grid.Item.Accessory[];
		};

		type Tag = string | Date | undefined | null | { color: ColorLike, value: string | Date | undefined | null };
		type Text = string | Date | undefined | null | { color: Color, value: string | Date | undefined | null };

		export type Accessory  = ({ tag?: Tag } | { text?: Text }) & {
				icon?: Image.ImageLike;
				tooltip?: string | null;
		};
	}
};

const GridRoot: React.FC<Grid.Props> = 
	({ onSearchTextChange, searchBarAccessory, onSelectionChange, children, actions, inset = Grid.Inset.Small, fit = Grid.Fit.Contain, aspectRatio = '1', ...props }) => {
	const searchTextChangeHandler = useEventListener(onSearchTextChange);
	const selectionChangeHandler = useEventListener(onSelectionChange);

	if (typeof props.enableFiltering === "boolean" && typeof props.filtering === "undefined") {
		props.filtering = props.enableFiltering;
	}

	return <grid 
		fit={fit}
		inset={inset}
		aspectRatio={aspectRatio}
		onSearchTextChange={searchTextChangeHandler} 
		onSelectionChange={selectionChangeHandler} 
		{...props}
	>
		{searchBarAccessory}
		{children}
		{actions}
	</grid>
}

const GridItem: React.FC<Grid.Item.Props> = ({ detail, actions, ...props }) => {
	const id = useRef(props.id ?? randomUUID());
	const nativeProps: React.JSX.IntrinsicElements['grid-item'] = {
		title: props.title,
		subtitle: props.subtitle,
		id: id.current,
	};

	if (props.icon) nativeProps.icon = serializeImageLike(props.icon);

	return (
		<grid-item {...nativeProps}>
			{detail}
			{actions}
		</grid-item>
	);
}

const GridSection: React.FC<Grid.Section.Props> = ({ fit = Grid.Fit.Contain, aspectRatio = '1', inset = Grid.Inset.Small, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['grid-section'] = {
		fit,
		aspectRatio,
		...props
	}

	return <grid-section {...nativeProps} />
}

export const ListAccessory: React.FC<Grid.Item.Accessory> = (props) => {
	return <list-accessory />
}

export const List = Object.assign(GridRoot, {
	Section: GridSection,
	EmptyView,
	Dropdown,
	Item: Object.assign(GridItem, {
		Accessory: ListAccessory
	}),
});
