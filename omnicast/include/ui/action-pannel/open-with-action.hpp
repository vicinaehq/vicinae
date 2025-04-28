#pragma once
#include "omni-icon.hpp"
#include "ui/action-pannel/action-section.hpp"

class OpenWithAction : public AbstractActionSection {
  std::vector<QString> args;

  std::vector<ActionItem> generateItems(const QString &query) const override;

public:
  OpenWithAction(const std::vector<QString> &args)
      : AbstractActionSection("Open with...", BuiltinOmniIconUrl("link")), args(args) {}
};
