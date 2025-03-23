#pragma once
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"

class AbstractActionSection : public AbstractAction {
public:
  virtual std::vector<ActionItem> generateItems(const QString &query) const = 0;
  void execute(AppWindow &app) override;
  void executePrelude(AppWindow &app) override;
  bool isPushView() const override { return true; }

  AbstractActionSection(const QString &title, const OmniIconUrl &icon) : AbstractAction(title, icon) {}
};
