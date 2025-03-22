#pragma once
#include "omni-icon.hpp"
#include "ui/keyboard-shortcut-indicator.hpp"
#include "ui/selectable-omni-list-widget.hpp"

class ActionListWidget : public SelectableOmniListWidget {
  OmniIcon *_icon;
  QLabel *_label;
  KeyboardShortcutIndicatorWidget *_shortcut;

public:
  ActionListWidget &setIconUrl(const OmniIconUrl &url);
  ActionListWidget &setShortcut(const KeyboardShortcutModel &shortcut);
  ActionListWidget &clearShortcut();
  ActionListWidget &setTitle(const QString &title);
  void selectionChanged(bool selected) override;

  ActionListWidget();
};
