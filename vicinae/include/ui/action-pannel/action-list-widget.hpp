#pragma once
#include "ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/image.hpp"
#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"

class ActionListWidget : public SelectableOmniListWidget {
  ImageWidget *m_icon;
  TypographyWidget *m_label;
  KeyboardShortcutIndicatorWidget *m_shortcut;

public:
  ActionListWidget &setIconUrl(const std::optional<ImageURL> &url);
  ActionListWidget &setShortcut(const Keyboard::Shortcut &shortcut);
  ActionListWidget &clearShortcut();
  ActionListWidget &setTitle(const QString &title);
  void selectionChanged(bool selected) override;
  void setAction(const AbstractAction *action);

  ActionListWidget();
};
