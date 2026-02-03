#include "extensions/clipboard/clipboard-extension.hpp"
#include "builtin_icon.hpp"
#include "single-view-command-context.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "service-registry.hpp"

class ClipboardClearCommand : public BuiltinCallbackCommand {
  QString id() const override { return "clear"; }
  QString name() const override { return "Clear Current Clipboard Data"; }
  QString description() const override { return "Clear the current content of the clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("delete-document").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController *ctrl) const override {
    auto ctx = ctrl->context();
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (!clipman->clear()) {
      toast->failure("Failed to clear clipboard");
      return;
    }

    ctx->navigation->showHud("Clipboard cleared", ImageURL::emoji("🤫"));
  }
};

class ClearClipboardHistoryCommand : public BuiltinCallbackCommand {
  QString id() const override { return "clear-history"; }
  QString name() const override { return "Clear Clipboard History"; }
  QString description() const override { return "Clear the clipboard history"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Trash).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController *ctrl) const override {
    auto ctx = ctrl->context();
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    ctx->navigation->confirmAlert("Are you sure?", "Your clipboard history will be gone forever :(",
                                  [clipman, toast]() {
                                    if (!clipman->removeAllSelections()) {
                                      toast->failure("Failed to clear clipboard history");
                                      return;
                                    }

                                    toast->success("Clipboard history cleared");
                                  });
  }
};

ClipboardExtension::ClipboardExtension() {
  registerCommand<ClipboardHistoryCommand>();
  registerCommand<ClipboardClearCommand>();
  registerCommand<ClearClipboardHistoryCommand>();
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
