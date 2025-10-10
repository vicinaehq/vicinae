#pragma once
#include "command-database.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <qlogging.h>
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"

class ClipboardExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return "Clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override { return "System clipboard integration"; }

  std::vector<Preference> preferences() const override {
    auto encryption = Preference::makeCheckbox("encryption");
    auto monitoring = Preference::makeCheckbox("monitoring");
    auto eraseOnStartup = Preference::makeCheckbox("eraseOnStartup");

    eraseOnStartup.setTitle("Erase on startup");
    eraseOnStartup.setDescription("Erase clipboard history every time the vicinae server is started");
    eraseOnStartup.setDefaultValue(false);

    encryption.setTitle("Disk encryption");
    encryption.setDescription("Whether to encrypt the clipboard data on disk. The "
                              "encryption key is stored and retrieved from the system keychain. Enabling "
                              "this might trigger your keychain's unlock dialog.");
    encryption.setDefaultValue(false);

    monitoring.setTitle("Clipboard monitoring");
    monitoring.setDescription("Whether clipboard activity is recorded in the history. Every clipboard action "
                              "performed while this is turned off will not be recorded.");
    monitoring.setDefaultValue(true);

    return {monitoring, encryption, eraseOnStartup};
  }

  virtual void initialized(const QJsonObject &preferences) const override {
    auto clipman = ServiceRegistry::instance()->clipman();
    bool eraseOnStartup = preferences.value("eraseOnStartup").toBool();

    if (eraseOnStartup) { clipman->removeAllSelections(); }
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    auto clipman = ServiceRegistry::instance()->clipman();

    clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
    clipman->setMonitoring(value.value("monitoring").toBool());
    clipman->setEncryption(value.value("encryption").toBool());
  }

  ClipboardExtension() { registerCommand<ClipboardHistoryCommand>(); }
};
