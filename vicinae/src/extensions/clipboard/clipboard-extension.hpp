#pragma once
#include "command-database.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "../../ui/image/url.hpp"
#include "vicinae.hpp"
#include <qlogging.h>

class ClipboardExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return "Clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override { return "System clipboard integration"; }

  std::vector<Preference> preferences() const override {
    auto monitoring = Preference::makeCheckbox("monitoring");

    monitoring.setTitle("Clipboard monitoring");
    monitoring.setDescription("Whether clipboard activity is recorded in the history. Every clipboard action "
                              "performed while this is turned off will not be recorded.");
    monitoring.setDefaultValue(true);

    auto storeAllOfferings = Preference::makeCheckbox("store-all-offerings");

    storeAllOfferings.setTitle("Store all offerings");
    storeAllOfferings.setDescription("Store and index alternative mime type offerings. This will "
                                     "increase total storage size, but will refine the search.");
    storeAllOfferings.setDefaultValue(true);

    return {monitoring, storeAllOfferings};
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    auto clipman = ServiceRegistry::instance()->clipman();

    clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
    clipman->setMonitoring(value.value("monitoring").toBool());
  }

  ClipboardExtension() { registerCommand<ClipboardHistoryCommand>(); }
};
