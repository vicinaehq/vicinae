#pragma once
#include "command-database.hpp"
#include "common.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "omni-icon.hpp"

class ClipboardExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "clipboard"; }
  QString name() const override { return "Clipboard"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("copy-clipboard").setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return "System clipboard integration"; }
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override {
    auto history = std::make_shared<ClipboardHistoryCommand>();

    return {history};
  }
};
