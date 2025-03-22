#pragma once
#include "app/app-database.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action-section.hpp"

class OpenWithAction : public AbstractActionSection {
  const AbstractAppDatabase &appDb;
  std::vector<QString> args;

  std::vector<ActionItem> generateItems(const QString &query) const override;

public:
  OpenWithAction(const std::vector<QString> &args, const AbstractAppDatabase &appDb)
      : AbstractActionSection("Open with...", BuiltinOmniIconUrl("link")), args(args), appDb(appDb) {}
};
