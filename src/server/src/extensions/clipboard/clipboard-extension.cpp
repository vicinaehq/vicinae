#include "extensions/clipboard/clipboard-extension.hpp"
#include <QCoreApplication>
#include "builtin_icon.hpp"
#include "single-view-command-context.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "service-registry.hpp"

class ClipboardClearCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ClipboardClearCommand)
  QString id() const override { return "clear"; }
  QString name() const override { return tr("Clear Current Clipboard Data"); }
  QString description() const override { return tr("Clear the current content of the clipboard"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("delete-document").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController &ctrl) const override {
    auto ctx = ctrl.context();
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    if (!clipman->clear()) {
      toast->failure(tr("Failed to clear clipboard"));
      return;
    }

    ctx->navigation->showHud(tr("Clipboard cleared"), ImageURL::emoji("🤫"));
  }
};

class ClearClipboardHistoryCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ClearClipboardHistoryCommand)
  QString id() const override { return "clear-history"; }
  QString name() const override { return tr("Clear Clipboard History"); }
  QString description() const override { return tr("Clear the clipboard history"); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Trash).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  void execute(CommandController &ctrl) const override {
    auto ctx = ctrl.context();
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    ctx->navigation->confirmAlert(tr("Are you sure?"), tr("Your clipboard history will be gone forever :("),
                                  [clipman, toast]() {
                                    if (!clipman->removeAllSelections()) {
                                      toast->failure(tr("Failed to clear clipboard history"));
                                      return;
                                    }

                                    toast->success(tr("Clipboard history cleared"));
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
  bool const eraseOnStartup = preferences.value("eraseOnStartup").toBool();

  if (eraseOnStartup) { clipman->removeAllSelections(); }
}

void ClipboardExtension::preferenceValuesChanged(const QJsonObject &value) const {
  auto clipman = ServiceRegistry::instance()->clipman();
  clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
  clipman->setMonitoring(value.value("monitoring").toBool());
#ifndef Q_OS_MACOS
  clipman->setIgnorePasswords(value.value("ignorePasswords").toBool());
#endif
}

std::vector<Preference> ClipboardExtension::preferences() const {
  auto monitoring = Preference::makeCheckbox("monitoring");
  auto eraseOnStartup = Preference::makeCheckbox("eraseOnStartup");

  eraseOnStartup.setTitle(tr("Erase on startup"));
  eraseOnStartup.setDescription(tr("Erase clipboard history every time the vicinae server is started"));
  eraseOnStartup.setDefaultValue(false);

  monitoring.setTitle(tr("Clipboard monitoring"));
  monitoring.setDescription(tr("Whether clipboard activity is recorded in the history. Every clipboard "
                               "action performed while this is turned off will not be recorded."));
  monitoring.setDefaultValue(true);

#ifdef Q_OS_MACOS
  return {monitoring, eraseOnStartup};
#else
  auto ignorePasswords = Preference::makeCheckbox("ignorePasswords");
  ignorePasswords.setDefaultValue(true);
  ignorePasswords.setTitle(tr("Ignore Passwords"));
  ignorePasswords.setDescription(
      tr("Ignore selections that can be identified as a password. This relies on the application providing "
         "an explicit hint that the selection is a password. While most password managers and private "
         "browser windows do, some might not implement this properly."));

  return {monitoring, ignorePasswords, eraseOnStartup};
#endif
}
