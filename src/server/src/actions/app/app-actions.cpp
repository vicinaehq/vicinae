#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-pannel/action.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/image/url.hpp"
#include <ranges>

OpenAppLocationAction::OpenAppLocationAction(const std::shared_ptr<AbstractApplication> &app,
                                             const std::shared_ptr<AbstractApplication> &opener)
    : AbstractAction("Open Location", opener->iconUrl()), m_app(app) {}

void OpenAppLocationAction::execute(ApplicationContext *ctx) {
  if (!ctx->services->appDb()->openLocation(*m_app)) {
    ctx->services->toastService()->failure("Failed to open app location");
    return;
  }

  ctx->navigation->closeWindow();
}

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
                             const std::vector<QString> &args)
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

OpenRawProgramAction::OpenRawProgramAction(const std::vector<QString> &args) : m_args(args) {}

QuitAppAction::QuitAppAction(const std::shared_ptr<AbstractApplication> &app)
    : AbstractAction("Quit Application", BuiltinIcon::XMarkCircle), m_app(app) {
  setAutoClose();
}

void QuitAppAction::execute(ApplicationContext *ctx) {
  auto toast = ctx->services->toastService();

  if (!ctx->services->appRuntime()->quit(*m_app)) {
    toast->failure(QString("Failed to quit %1").arg(m_app->displayName()));
    return;
  }

  ctx->navigation->showHud(QString("Quit %1").arg(m_app->displayName()));
}

ForceQuitAppAction::ForceQuitAppAction(const std::shared_ptr<AbstractApplication> &app)
    : AbstractAction("Force Quit Application", BuiltinIcon::XMarkCircle), m_app(app) {
  setAutoClose();
}

void ForceQuitAppAction::execute(ApplicationContext *ctx) {
  auto toast = ctx->services->toastService();

  if (!ctx->services->appRuntime()->forceQuit(*m_app)) {
    toast->failure(QString("Failed to force quit %1").arg(m_app->displayName()));
    return;
  }

  ctx->navigation->showHud(QString("Force quit %1").arg(m_app->displayName()));
}

void OpenInBrowserAction::execute(ApplicationContext *ctx) {
  const auto toast = ctx->services->toastService();

  if (!ctx->services->appDb()->openTarget(m_url)) {
    toast->failure("Failed to open toast");
    return;
  }

  ctx->navigation->showHud("Opened in browser");
}

OpenWithAction::OpenWithAction(QString target, const AppService &db)
    : ListSubmenuAction("Open with...", BuiltinIcon::ArrowUp), m_db(db), m_target(std::move(target)) {
  setShortcut(Keybind::OpenAction);
}

std::unique_ptr<ActionPanelState> OpenWithAction::buildState() const {
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();

  for (const auto &opener : m_db.findOpeners(m_target)) {
    auto action = new OpenAppAction(opener, opener->displayName(), {m_target});
    section->addAction(action);
  }

  return panel;
}
