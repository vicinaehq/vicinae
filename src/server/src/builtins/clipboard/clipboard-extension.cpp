#include "builtins/clipboard/clipboard-extension.hpp"
#include <QCoreApplication>
#include "services/builtin-icon/builtin-icon.hpp"
#include "command/single-view-command-context.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "builtins/clipboard/clipboard-history-command.hpp"
#include "service-registry.hpp"

class ClipboardClearCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ClipboardClearCommand)
  QString id() const override { return "clear"; }
  QString name() const override { return tr("Clear Current Clipboard Data"); }
  QString description() const override { return tr("Clear the current content of the clipboard"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::CopyClipboard)
        .setBackgroundTint(SemanticColor::Red)
        .setBadge(BuiltinIcon::Xmark);
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
    return ImageURL::builtin(BuiltinIcon::CopyClipboard)
        .setBackgroundTint(SemanticColor::Red)
        .setBadge(BuiltinIcon::Xmark);
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

void ClipboardExtension::initialized(const ClipboardPreferences &preferences) const {
  if (preferences.eraseOnStartup) ServiceRegistry::instance()->clipman()->removeAllSelections();
}

void ClipboardExtension::preferencesChanged(const ClipboardPreferences &preferences) const {
  auto clipman = ServiceRegistry::instance()->clipman();

  clipman->setMonitoring(preferences.monitoring);
  clipman->setIgnoredApps(preferences.ignoredApps);
  clipman->setHistoryEvictionThreshold(evictionThreshold(preferences.evictionThreshold),
                                       preferences.preserveTagged);
#ifndef Q_OS_MACOS
  clipman->setIgnorePasswords(preferences.ignorePasswords);
#endif
}
