#pragma once
#include <cstdint>
#include <qcolor.h>
#include <qstring.h>
#include <variant>

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
  ListItemSecondaryHoverForeground,
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
  InputBackground,
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

struct DynamicColor {
  QString light;
  QString dark;
  bool adjustContrast = false;
};

using ColorLike = std::variant<QColor, QString, SemanticColor, DynamicColor>;
