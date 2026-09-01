#include "extensions/clipboard/clipboard-extension.hpp"
#include <QCoreApplication>
#include <chrono>
#include "builtin_icon.hpp"
#include "numen/numen.hpp"
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
    return ImageURL::builtin(BuiltinIcon::DeleteDocument).setBackgroundTint(SemanticColor::Red);
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
    return ImageURL(BuiltinIcon::Trash).setBackgroundTint(SemanticColor::Red);
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
  auto parseEvictionThreshold = [](const QString &text) -> std::optional<std::chrono::seconds> {
    if (text == "never") return std::nullopt;
    auto parsed = numen::Numen{}.parse<std::chrono::seconds>(text.toStdString());
    if (!parsed) return std::nullopt;
    return *parsed;
  };

  auto clipman = ServiceRegistry::instance()->clipman();
  auto evictionThreshold = parseEvictionThreshold(value.value("evictionThreshold").toString());
  auto preservedTaggedSelections = value.value("preserveTagged").toBool();

  clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
  clipman->setMonitoring(value.value("monitoring").toBool());
  clipman->setHistoryEvictionThreshold(evictionThreshold, preservedTaggedSelections);

#ifndef Q_OS_MACOS
  clipman->setIgnorePasswords(value.value("ignorePasswords").toBool());
#endif
}

std::vector<Preference> ClipboardExtension::preferences() const {
  std::vector<Preference::DropdownData::Option> evictionThresholdPresets = {
      {tr("Never"), "never"},   {tr("15 minutes"), "15 minutes"}, {tr("1 hour"), "1 hour"},
      {tr("1 day"), "1 day"},   {tr("1 week"), "1 week"},         {tr("1 month"), "1 month"},
      {tr("1 year"), "1 year"},
  };

  auto monitoring = Preference::makeCheckbox("monitoring");
  auto evictionThreshold = Preference::makeDropdown("evictionThreshold", evictionThresholdPresets);
  auto preserveTaggedSelections = Preference::makeCheckbox("preserveTagged");
  auto eraseOnStartup = Preference::makeCheckbox("eraseOnStartup");

  evictionThreshold.setTitle("Eviction threshold");
  evictionThreshold.setDescription("Automatically delete selections older than this threshold");
  evictionThreshold.setDefaultValue("never");

  preserveTaggedSelections.setTitle("Preserve tagged");
  preserveTaggedSelections.setDescription(
      "Never evict or mass delete selections that have been explicitly tagged (pinned, custom keyword).");
  preserveTaggedSelections.setDefaultValue(true);

  eraseOnStartup.setTitle(tr("Erase on startup"));
  eraseOnStartup.setDescription(tr("Erase clipboard history every time the vicinae server is started"));
  eraseOnStartup.setDefaultValue(false);

  monitoring.setTitle(tr("Clipboard monitoring"));
  monitoring.setDescription(tr("Whether clipboard activity is recorded in the history. Every clipboard "
                               "action performed while this is turned off will not be recorded."));
  monitoring.setDefaultValue(true);

#ifdef Q_OS_MACOS
  return {monitoring, preserveTaggedSelections, evictionThreshold, eraseOnStartup};
#else
  auto ignorePasswords = Preference::makeCheckbox("ignorePasswords");
  ignorePasswords.setDefaultValue(true);
  ignorePasswords.setTitle(tr("Ignore Passwords"));
  ignorePasswords.setDescription(
      tr("Ignore selections that can be identified as a password. This may not work with all apps."));

  return {monitoring, ignorePasswords, preserveTaggedSelections, evictionThreshold, eraseOnStartup};
#endif
}
