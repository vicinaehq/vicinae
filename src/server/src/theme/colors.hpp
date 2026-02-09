#pragma once
#include <cstdint>

enum SemanticColor : std::uint8_t {

  // Basic color palette
  Blue,
  Green,
  Magenta,
  Orange,
  Red,
  Yellow,
  Cyan,
  Purple, // for now, aliased to magenta

  MainBackground,
  MainWindowBorder,
  SettingsWindowBorder,

  StatusBarBackground,

  // Background colors
  Background,
  BackgroundBorder,
  SecondaryBackground,
  SecondaryBackgroundBorder,

  ListItemSelectionBackground,
  ListItemSelectionForeground,
  ListItemHoverBackground,
  ListItemHoverForegroud,
  ListItemSecondaryHoverBackground,
  ListItemSecondarySelectionBackground,
  ListItemSecondarySelectionForeground,

  ButtonPrimaryBackground,
  ButtonPrimaryForeground,
  ButtonPrimaryHoverBackground,
  ButtonPrimaryHoverForeground,
  ButtonPrimaryFocusOutline,

  GridItemSelectionOutline,
  GridItemHoverOutline,
  GridItemBackground,

  PopoverBackground,
  PopoverBorder,

  TooltipBackground,
  TooltipForeground,
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

  TextPrimary,
  TextMuted,
  TextDanger,
  TextSuccess,
  TextPlaceholder,

  // link
  LinkDefault,
  LinkVisited,

  InvalidTint,

  ShortcutIndicatorBorder,
};
