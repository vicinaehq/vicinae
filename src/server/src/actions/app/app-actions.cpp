#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

void OpenInTerminalAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  m_opts.emulator = m_emulator.get();

  if (!appDb->launchTerminalCommand(m_args, m_opts)) {
    toast->setToast("Failed to start app", ToastStyle::Danger);
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenInTerminalAction::OpenInTerminalAction(const std::shared_ptr<AbstractApplication> &emulator,
                                           const std::vector<QString> &cmdline,
                                           const LaunchTerminalCommandOptions &opts)
    : m_emulator(emulator), m_args(cmdline), m_opts(opts) {}

void OpenAppAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (!appDb->launch(*application, args)) {
    toast->setToast("Failed to start app", ToastStyle::Danger);
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenAppAction::OpenAppAction(const std::shared_ptr<AbstractApplication> &app, const QString &title,
                             const std::vector<QString> args)
    : AbstractAction(title, app->iconUrl()), application(app), args(args) {}

void OpenRawProgramAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  if (!appDb->launchRaw(m_args)) {
    toast->failure("Failed to start app");
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenRawProgramAction::OpenRawProgramAction(const std::vector<QString> args) : m_args(args) {}

void OpenInBrowserAction::execute(ApplicationContext *ctx) {
  const auto toast = ctx->services->toastService();

  if (!ctx->services->appDb()->openTarget(m_url)) {
    toast->failure("Failed to open toast");
    return;
  }

  ctx->navigation->showHud("Opened in browser");
}
