export namespace Color {
  export type Raw = string;
}

export enum Color {
  // Basic semantic colors - aligned with C++ SemanticColor enum
  Blue = "blue",
  Green = "green",
  Magenta = "magenta",
  Orange = "orange",
  Purple = "purple",
  Red = "red",
  Yellow = "yellow",
  Cyan = "cyan",

  // Text colors - aligned with C++ semantic system
  TextPrimary = "text-primary",
  TextSecondary = "text-secondary",
  TextTertiary = "text-tertiary",
  TextDisabled = "text-disabled",
  TextOnAccent = "text-on-accent",
  TextError = "text-error",
  TextSuccess = "text-success",
  TextWarning = "text-warning",

  // Background colors
  MainBackground = "main-background",
  MainHoverBackground = "main-hover-background",
  MainSelectedBackground = "main-selected-background",
  SecondaryBackground = "secondary-background",
  TertiaryBackground = "tertiary-background",

  // Button states
  ButtonPrimary = "button-primary",
  ButtonPrimaryHover = "button-primary-hover",
  ButtonPrimaryPressed = "button-primary-pressed",
  ButtonPrimaryDisabled = "button-primary-disabled",

  ButtonSecondary = "button-secondary",
  ButtonSecondaryHover = "button-secondary-hover",
  ButtonSecondaryPressed = "button-secondary-pressed",
  ButtonSecondaryDisabled = "button-secondary-disabled",

  ButtonDestructive = "button-destructive",
  ButtonDestructiveHover = "button-destructive-hover",
  ButtonDestructivePressed = "button-destructive-pressed",

  // Input/form states
  InputBackground = "input-background",
  InputBorder = "input-border",
  InputBorderFocus = "input-border-focus",
  InputBorderError = "input-border-error",
  InputPlaceholder = "input-placeholder",

  // UI elements
  Border = "border",
  BorderSubtle = "border-subtle",
  BorderStrong = "border-strong",
  Separator = "separator",
  Shadow = "shadow",

  // Status/feedback colors
  StatusBackground = "status-background",
  StatusBorder = "status-border",
  StatusHover = "status-hover",

  ErrorBackground = "error-background",
  ErrorBorder = "error-border",
  SuccessBackground = "success-background",
  SuccessBorder = "success-border",
  WarningBackground = "warning-background",
  WarningBorder = "warning-border",

  // Interactive elements
  LinkDefault = "link-default",
  LinkHover = "link-hover",
  LinkVisited = "link-visited",

  // Special states
  Focus = "focus",
  Overlay = "overlay",
  Tooltip = "tooltip",
  TooltipText = "tooltip-text",
}

export type ColorLike = Color.Raw | Color;
export type SerializedColorLike = string;

export const serializeColorLike = (color: ColorLike): SerializedColorLike => {
  if (typeof color == "string") {
    return color;
  }

  return color;
};
