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
  SecondaryBackground,
  SecondaryBackgroundBorder,

  SelectionBackground,
  SelectionForeground,
  HoverBackground,
  HoverForegroud,
  SecondarySelectionBackground,
  SecondarySelectionForeground,

  PopoverBackground,
  PopoverBorder,

  TooltipBackground,
  TooltipBorder,

  LoadingBar,
  DynamicToastSpinner,

  ScrollBarBackground,
  SecondaryScrollBarBackground,

  // foreground
  Foreground,
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
  TextMuted,
  TextDanger,
  TextSuccess,

  // link
  LinkDefault,
  LinkVisited,

  InvalidTint,
};
