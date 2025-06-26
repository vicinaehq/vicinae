#pragma once
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/keyboard-shortcut-indicator.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"

class ActionListWidget : public SelectableOmniListWidget {
  OmniIcon *m_icon;
  TypographyWidget *m_label;
  KeyboardShortcutIndicatorWidget *m_shortcut;

public:
  ActionListWidget &setIconUrl(const OmniIconUrl &url);
  ActionListWidget &setShortcut(const KeyboardShortcutModel &shortcut);
  ActionListWidget &clearShortcut();
  ActionListWidget &setTitle(const QString &title);
  void selectionChanged(bool selected) override;
  void setAction(const AbstractAction *action);

  ActionListWidget();
};
