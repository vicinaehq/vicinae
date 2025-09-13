#pragma once
#include "../../../src/ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/image.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"

class ActionListWidget : public SelectableOmniListWidget {
  ImageWidget *m_icon;
  TypographyWidget *m_label;
  KeyboardShortcutIndicatorWidget *m_shortcut;
  AbstractAction::Style m_actionStyle = AbstractAction::Style::Normal;

public:
  ActionListWidget &setIconUrl(const ImageURL &url);
  ActionListWidget &setShortcut(const KeyboardShortcutModel &shortcut);
  ActionListWidget &clearShortcut();
  ActionListWidget &setTitle(const QString &title);
  void selectionChanged(bool selected) override;
  void setAction(const AbstractAction *action);

  ActionListWidget();
};
