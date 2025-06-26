#pragma once
#include "common.hpp"

class ClipboardHistoryCommand : public AbstractCmd {
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
  std::vector<Preference> preferences() const override {
    auto storeAllOfferings = Preference::makeCheckbox();

    storeAllOfferings.setName("store-all-offerings");
    storeAllOfferings.setTitle("Store all offerings");
    storeAllOfferings.setDescription("Store and index alternative mime type offerings. This will "
                                     "increase total storage size, but will refine the search.");

    auto maxStorageSize = Preference::makeText();

    maxStorageSize.setName("maximum-storage-size");
    maxStorageSize.setTitle("Maximum storage size");
    maxStorageSize.setDescription("How much storage can be used to store clipboard history data, in MB.");
    maxStorageSize.setDefaultValue("1000");

    return {storeAllOfferings, maxStorageSize};
  }
  void preferenceValuesChanged(const QJsonValue &value) override;
  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override;
};
