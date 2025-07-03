#pragma once
#include "clipboard-history-view.hpp"
#include "common.hpp"
#include "single-view-command-context.hpp"
#include <qjsonobject.h>

class ClipboardHistoryCommand : public AbstractViewCommand<ClipboardHistoryView> {
  QString uniqueId() const override { return "clipboard.clipboard-history"; }
  QString name() const override { return "Clipboard History"; }
  QString description() const override {
    return "Browse your clipboard's history, pin, edit and remove entries.";
  }
  QString extensionId() const override { return "clipboard"; }
  QString commandId() const override { return "clipboard-history"; }
  virtual CommandMode mode() const override { return CommandMode::CommandModeView; }
  virtual CommandType type() const override { return CommandType::CommandTypeBuiltin; }
  OmniIconUrl iconUrl() const override {
    auto icon = BuiltinOmniIconUrl("copy-clipboard");
    icon.setBackgroundTint(ColorTint::Red);
    return icon;
  }
  std::vector<Preference> preferences() const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;
};
