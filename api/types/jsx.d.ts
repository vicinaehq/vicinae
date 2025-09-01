import * as React from "react";
 
import type { ListItemDetailProps } from "../api/components/list";
import { SerializedImageLike } from "../api/image";
import { SerializedColorLike } from "../api/color";
import { Keyboard } from "../api/keyboard";
import { Grid } from "../api/components/grid";

import 'react';

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
        icon?: SerializedImageLike;
        keywords?: string[];
      };
      "list-item-detail": ListItemDetailProps;
      "list-item-detail-metadata": any;
      "list-accessory": {};

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
        fit: Grid.Fit;
        aspectRatio: Grid.AspectRatio;

        title?: string;
        subtitle?: string;
        children?: React.ReactNode;
      };
      "grid-item": {
        title?: string;
        id?: string;
        subtitle?: string;
        content?: SerializedImageLike | { color: ColorLike } | { value: SerializedImageLike, tooltip?: string };
        keywords?: string[];
      };

      "empty-view": {
        description?: string;
        title?: string;
        icon?: SerializedImageLike;
      };
      metadata: {
        children?: React.ReactNode;
      };
      "metadata-label": {
        title: string;
        text: string;
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
      };
      "action-panel-submenu": {
        title: string;
        icon?: SerializedImageLike;
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
        icon?: SerializedImageLike;
        autoFocus?: boolean;
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
      "text-field": {
        onBlur?: Function;
        onFocus?: Function;
        onChange?: Function;
      };
      "dropdown-field": {
        onBlur?: Function;
        onFocus?: Function;
        onChange?: Function;
        children?: ReactNode;
      };
      "date-picker-field": {};
      "checkbox-field": {
        onBlur?: Function;
        onFocus?: Function;
        onChange?: Function;
      };
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
        icon?: SerializedImageLike;
        keywords?: string[];
      };

      separator: {};
      "menu-bar": {};
      "menu-bar-item": {};
      "menu-bar-submenu": {};
      "menu-bar-section": {};
    }
  }
}
