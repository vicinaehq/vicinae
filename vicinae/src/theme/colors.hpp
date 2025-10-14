#pragma once

enum SemanticColor {

  // Basic color palette
  Blue,
  Green,
  Magenta,
  Orange,
  Red,
  Yellow,
  Cyan,
  Purple, // for now, aliased to magenta

  MainWindowBorder,
  SettingsWindowBorder,

  // Background colors
  Background,
  BackgroundBorder,
  LighterBackground,
  LighterBackgroundBorder,
  SelectionBackground,
  HoverBackground,
  LighterSelectionBackground,
  LighterHoverBackground,

  PopoverBackground,
  PopoverBorder,

  TooltipBackground,
  TooltipBorder,

  LoadingBar,
  DynamicToastSpinner,

  ScrollBarBackground,

  // foreground
  Foreground,
  LightForeground,
  DarkForeground,
  LightestForeground,
  AccentForeground,

  Accent,

  // text selection
  TextSelectionBackground,
  TextSelectionForeground,

  // input
  InputBorder,
  InputBorderFocus,
  InputBorderError,
  InputPlaceholder,

  TextPrimary,
  TextSecondary,
  TextError,
  TextSuccess,

  // link
  LinkDefault,
  LinkVisited,

  InvalidTint,
};
