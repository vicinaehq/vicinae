import * as React from "react";
 
import type { ListItemDetailProps } from "../api/components/list";
import { ImageLike } from "../api/image";
import { SerializedColorLike } from "../api/color";
import { Keyboard } from "../api/keyboard";
import { Grid } from "../api/components/grid";

import 'react';
import { ImageLike, List } from "../src";

type BaseFormField = {
    onBlur?: Function;
    onFocus?: Function;
    onChange?: Function;
};

declare module "react" {
  namespace JSX {
    interface IntrinsicElements {
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
        icon?: ImageLike;
        keywords?: string[];
		children?: ReactNode;
      };
      "list-item-detail": ListItemDetailProps;
      "list-item-detail-metadata": any;

      grid: {
        inset?: Grid.Inset;
        columns?: number;
        fit: Grid.Fit;
        aspectRatio: Grid.AspectRatio;

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
        aspectRatio?: Grid.AspectRatio;

        title?: string;
        subtitle?: string;
        children?: React.ReactNode;
      };
      "grid-item": {
        title?: string;
        id?: string;
        subtitle?: string;
        content?: ImageLike | { color: ColorLike } | { value: ImageLike, tooltip?: string };
        keywords?: string[];
		children?: ReactNode;
      };

      "empty-view": {
        description?: string;
        title?: string;
        icon?: ImageLike;
      };
      metadata: {
        children?: React.ReactNode;
      };
      "metadata-label": {
        title: string;
        text: string;
        icon?: ImageLike;
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
      };
      "action-panel-submenu": {
        title: string;
        icon?: ImageLike;
        onOpen?: () => void;
        onSearchTextChange?: (text: string) => void;
        children?: React.ReactNode;
      };
      "action-panel-section": {
        title?: string;
        children?: React.ReactNode;
      };
      action: {
        title: string;
        onAction: () => void;
        onSubmit?: Function;
        shortcut?: Keyboard.Shortcut;
        icon?: ImageLike;
        autoFocus?: boolean;
      };
      "tag-list": {
        title?: string;
        children?: React.ReactNode;
      };
      "tag-item": {
        color?: SerializedColorLike;
        icon?: ImageLike;
        text?: string;
        onAction?: () => void;
      };
      form: {
        enableDrafts: boolean;
        isLoading: boolean;
        navigationTitle?: string;
        children?: React.ReactNode;
      };
      "text-field": BaseFormField & {};
	  "tag-picker-field": BaseFormField & {},
	  "tag-picker-item": {
		  title: string;
		  value: string;
		  icon?: ImageLike;
	  },
	  "text-area-field": BaseFormField & {
	  }
	  "file-picker-field": BaseFormField & {
	  }
      "dropdown-field": BaseFormField & {
        children?: ReactNode;
      };
      "date-picker-field": {};
      "checkbox-field": BaseFormField & {};
      "password-field": {};
      "textarea-field": {};

      dropdown: {
        onChange?: Function;
        onSearchTextChange?: (text: string) => void;
        children?: ReactNode;
      };
      "dropdown-section": {
        title?: string;
        children: ReactNode;
      };
      "dropdown-item": {
        title: string;
        value: string;
        icon?: ImageLike;
        keywords?: string[];
      };

	  "form-description": {
		  title?: string;
		  text: string;
	  }

      separator: {};
      "menu-bar": {};
      "menu-bar-item": {};
      "menu-bar-submenu": {};
      "menu-bar-section": {};
    }
  }
}
