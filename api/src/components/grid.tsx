import React, { ReactNode, useRef } from 'react';
import { Image, ImageLike, serializeImageLike } from '../image';
import { randomUUID } from 'crypto';
import { EmptyView } from './empty-view';
import { useEventListener } from '../hooks';
import { Color, ColorLike } from '../color';
import { Dropdown } from './dropdown';

enum GridInset {
	Small = 'small',
	Medium = 'medium',
	Large = 'large',
}

type SectionConfig = {
	inset?: GridInset;
	columns?: number;
	fit?: GridFit;
	aspectRatio?: GridAspectRatio;
}

type GridAspectRatio = '1' | '3/2' | '2/3' | '4/3' | '3/4' | '16/9' | '9/16';

enum GridFit {
	Contain = 'contain',
	Fill = 'fill'
}

export declare namespace Grid {
	export namespace Section {
		export type Props = GridSectionProps; 
	}

	export type Props = GridProps;
	export type Inset = GridInset;
	export type AspectRatio = GridAspectRatio;

	export namespace Item {
		export type Props = {
			title?: string;
			detail?: React.ReactNode;
			keywords?: string[];
			icon?: ImageLike;
			content: Image.ImageLike | { color: ColorLike };
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

type GridProps = SectionConfig & {
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


type GridSectionProps = SectionConfig & {
		title?: string;
		subtitle?: string;
		children?: ReactNode;
};

const GridRoot: React.FC<GridProps> = 
	({ onSearchTextChange, searchBarAccessory, onSelectionChange, children, actions, inset = GridInset.Small, fit = GridFit.Contain, aspectRatio = '1', ...props }) => {
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

const GridItem: React.FC<Grid.Item.Props> = ({ detail, actions, keywords, ...props }) => {
	const id = useRef(props.id ?? randomUUID());
	const nativeProps: React.JSX.IntrinsicElements['grid-item'] = {
		title: props.title,
		subtitle: props.subtitle,
		id: id.current,
		keywords,
	};
	const isColor = (content: Grid.Item.Props['content']): content is { color: ColorLike } => {
		return !!content['color'];
	}

	if (isColor(props.content)) {
		nativeProps.content = { color: props.content.color };
	} else {
		nativeProps.content = serializeImageLike(props.content);
	}

	return (
		<grid-item {...nativeProps}>
			{detail}
			{actions}
		</grid-item>
	);
}

const GridSection: React.FC<Grid.Section.Props> = ({ fit = GridFit.Contain, aspectRatio = '1', inset = GridInset.Small, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['grid-section'] = {
		fit,
		aspectRatio,
		...props
	}

	return <grid-section {...nativeProps} />
}

export const GridAccessory: React.FC<Grid.Item.Accessory> = (props) => {
	return <list-accessory />
}

export const Grid = Object.assign(GridRoot, {
	Section: GridSection,
	EmptyView,
	Dropdown,
	Item: Object.assign(GridItem, {
		Accessory: GridAccessory
	}),
});
