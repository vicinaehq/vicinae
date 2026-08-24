#pragma once
#include <cstdint>

enum class Keybind : uint8_t {
  ToggleActionPanel = 0,
  OpenSearchAccessorySelector,
  OpenSettings,

  // common action keybinds
  OpenAction,
  CopyAction,
  CopyNameAction,
  CopyPathAction,
  SaveAction,
  DuplicateAction,
  PasteAction,
  NewAction,
  PinAction,
  FavoriteAction,
  RemoveAction,
  DangerousRemoveAction,
  EditAction,
  EditSecondaryAction,
  MoveUpAction,
  MoveDownAction,
  RefreshAction,

  KeybindEnd,
};
