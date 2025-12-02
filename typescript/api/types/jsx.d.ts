import type * as React from "react";
import type {
	DatePickerType,
	Grid,
	Keyboard,
	List,
	Quicklink,
	SerializedColorLike,
	SerializedImageLike,
} from "../src";

type BaseFormField = {
	onBlur?: Function;
	onFocus?: Function;
	onChange?: Function;
};

declare module "react" {
	namespace JSX {
		interface IntrinsicElements {
			view: {
				children: React.ReactNode;
			};
			detail: {
				navigationTitle?: string;
				markdown: string;
			};
			list: {
				children?: React.ReactNode;
				filtering?: boolean;
				isLoading?: boolean;
				isShowingDetail?: boolean;
				searchBarPlaceholder?: string;
				navigationTitle?: string;
				onSearchTextChange?: (text: string) => void;
				onSelectionChange?: (selectedItemId: string) => void;
			};
			"list-section": {
				title?: string;
				subtitle?: string;
				children?: React.ReactNode;
			};
			"list-item": {
				title: string;
				id?: string;
				subtitle?: string;
				icon?:
				| SerializedImageLike
				| {
					value?: SerializedImageLike | null;
					tooltip: string;
				};
				keywords?: string[];
				accessories?: List.Item.SerializedAccessory[];
				children?: React.ReactNode;
			};
			"list-item-detail": List.Item.Detail.Props;
			"list-item-detail-metadata": any;

			grid: {
				inset?: Grid.Inset;
				columns?: number;
				fit: Grid.Fit;
				aspectRatio: number;

				children?: React.ReactNode;
				filtering?: boolean;
				isLoading?: boolean;
				isShowingDetail?: boolean;
				searchBarPlaceholder?: string;
				navigationTitle?: string;
				onSearchTextChange?: (text: string) => void;
				onSelectionChange?: (selectedItemId: string) => void;
			};
			"grid-section": {
				inset?: Grid.Inset;
				columns?: number;
				fit?: Grid.Fit;
				aspectRatio?: number;

				title?: string;
				subtitle?: string;
				children?: React.ReactNode;
			};
			"grid-item": {
				title?: string;
				id?: string;
				subtitle?: string;
				content?: SerializedImageLike | { color: SerializedColorLike };
				tooltip?: string;
				keywords?: string[];
				accessory?: {
					icon?: SerializedImageLike;
					tooltip?: string | null;
				};
				children?: React.ReactNode;
			};

			"empty-view": {
				description?: string;
				title?: string;
				icon?: SerializedImageLike;
				children?: React.ReactNode;
			};
			metadata: {
				children?: React.ReactNode;
			};
			"metadata-label": {
				title: string;
				text: string | { color?: SerializedColorLike; value: string };
				icon?: SerializedImageLike;
			};
			"metadata-separator": {};
			"metadata-link": {
				title: string;
				target: string;
				text: string;
			};
			"action-panel": {
				title?: string;
				children?: React.ReactNode;
				stableId?: string;
			};
			"action-panel-submenu": {
				title: string;
				icon?: SerializedImageLike;
				shortcut?: Keyboard.Shortcut | Keyboard.Shortcut.Common;
				autoFocus?: boolean;
				filtering?: boolean | { keepSectionOrder: boolean };
				isLoading?: boolean;
				throttle?: boolean;
				onOpen?: () => void;
				onSearchTextChange?: (text: string) => void;
				children?: React.ReactNode;
				stableId?: string;
			};
			"action-panel-section": {
				title?: string;
				children?: React.ReactNode;
				stableId?: string;
			};
			action: {
				title: string;
				onAction: () => void;
				onSubmit?: Function;
				shortcut?: Keyboard.Shortcut | Keyboard.Shortcut.Common;
				icon?: SerializedImageLike;
				autoFocus?: boolean;
				type?: string;
				quicklink?: Quicklink;
				stableId?: string;
			};
			"tag-list": {
				title?: string;
				children?: React.ReactNode;
			};
			"tag-item": {
				color?: SerializedColorLike;
				icon?: SerializedImageLike;
				text?: string;
				onAction?: () => void;
			};
			form: {
				enableDrafts: boolean;
				isLoading: boolean;
				navigationTitle?: string;
				children?: React.ReactNode;
			};
			"link-accessory": {
				target: string;
				text: string;
			};
			"text-field": BaseFormField & {};
			"tag-picker-field": BaseFormField & {};
			"tag-picker-item": {
				title: string;
				value: string;
				icon?: SerializedImageLike;
			};
			"text-area-field": BaseFormField & {};
			"file-picker-field": BaseFormField & {
				allowMultipleSelection?: boolean;
				canChooseDirectories?: boolean;
				canChooseFiles?: boolean;
				showHiddenFiles?: boolean;
			};
			"dropdown-field": BaseFormField & {
				children?: React.ReactNode;
			};
			"date-picker-field": BaseFormField & {
				min?: Date;
				max?: Date;
				type?: DatePickerType;
			};
			"checkbox-field": BaseFormField & {};
			"password-field": {};
			"textarea-field": {};

			dropdown: {
				onChange?: Function;
				onSearchTextChange?: (text: string) => void;
				children?: React.ReactNode;
			};
			"dropdown-section": {
				title?: string;
				children: React.ReactNode;
			};
			"dropdown-item": {
				title: string;
				value: string;
				icon?: SerializedImageLike;
				keywords?: string[];
			};

			"form-description": {
				title?: string;
				text: string;
			};

			separator: {};
			"menu-bar": {};
			"menu-bar-item": {};
			"menu-bar-submenu": {};
			"menu-bar-section": {};
		}
	}
}
