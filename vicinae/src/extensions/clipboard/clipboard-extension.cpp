#include "extensions/clipboard/clipboard-extension.hpp"
#include "single-view-command-context.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"

class ClipboardClearCommand : public BuiltinCallbackCommand {
  QString id() const override { return "clear"; }
  QString name() const override { return "Clear Clipboard"; }
  QString description() const override { return "Clear the content of your system clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("delete-document").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController *ctrl) const override {
    auto ctx = ctrl->context();
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (!clipman->clear()) { return toast->failure("Failed to clear clipboard"); }

    ctx->navigation->showHud("Clipboard cleared", ImageURL::emoji("🤫"));
  }
};

ClipboardExtension::ClipboardExtension() {
  registerCommand<ClipboardHistoryCommand>();
  registerCommand<ClipboardClearCommand>();
}

void ClipboardExtension::initialized(const QJsonObject &preferences) const {
  auto clipman = ServiceRegistry::instance()->clipman();
  bool eraseOnStartup = preferences.value("eraseOnStartup").toBool();

  if (eraseOnStartup) { clipman->removeAllSelections(); }
}

void ClipboardExtension::preferenceValuesChanged(const QJsonObject &value) const {
  auto clipman = ServiceRegistry::instance()->clipman();
  clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
  clipman->setMonitoring(value.value("monitoring").toBool());
  clipman->setEncryption(value.value("encryption").toBool());
  clipman->setIgnorePasswords(value.value("ignorePasswords").toBool());
}

std::vector<Preference> ClipboardExtension::preferences() const {
  auto encryption = Preference::makeCheckbox("encryption");
  auto monitoring = Preference::makeCheckbox("monitoring");
  auto eraseOnStartup = Preference::makeCheckbox("eraseOnStartup");
  auto ignorePasswords = Preference::makeCheckbox("ignorePasswords");

  eraseOnStartup.setTitle("Erase on startup");
  eraseOnStartup.setDescription("Erase clipboard history every time the vicinae server is started");
  eraseOnStartup.setDefaultValue(false);

  ignorePasswords.setDefaultValue(true);
  ignorePasswords.setTitle("Ignore Passwords");
  ignorePasswords.setDescription(
      "Ignore selections that can be identified as a password. This relies on the application providing an "
      "explicit hint that the selection is a password. While most password managers and private browser "
      "windows do, some might not implement this properly.");

  encryption.setTitle("Disk encryption");
  encryption.setDescription("Whether to encrypt the clipboard data on disk. The "
                            "encryption key is stored and retrieved from the system keychain. Enabling "
                            "this might trigger your keychain's unlock dialog.");
  encryption.setDefaultValue(false);

  monitoring.setTitle("Clipboard monitoring");
  monitoring.setDescription("Whether clipboard activity is recorded in the history. Every clipboard action "
                            "performed while this is turned off will not be recorded.");
  monitoring.setDefaultValue(true);

  return {monitoring, ignorePasswords, eraseOnStartup, encryption};
}
