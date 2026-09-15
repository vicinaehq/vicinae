#include "actions/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-panel/action.hpp"
#include "services/app-runtime/app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/image/url.hpp"
#include <QTimer>
#include <iterator>
#include <ranges>

OpenAppLocationAction::OpenAppLocationAction(const std::shared_ptr<AbstractApplication> &app,
                                             const std::shared_ptr<AbstractApplication> &opener)
    : AbstractAction(tr("Open Location"), opener->iconUrl()), m_app(app) {}

void OpenAppLocationAction::execute(ApplicationContext *ctx) {
  if (!ctx->services->appDb()->openLocation(*m_app)) {
    ctx->services->toastService()->failure(tr("Failed to open app location"));
    return;
  }

  ctx->navigation->closeWindow();
}

void OpenInTerminalAction::execute(ApplicationContext *ctx) {
  auto appDb = ctx->services->appDb();
  auto toast = ctx->services->toastService();

  m_opts.emulator = m_emulator.get();

  if (!appDb->launchTerminalCommand(m_args, m_opts)) {
    toast->setToast(tr("Failed to start app"), ToastStyle::Danger);
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
    toast->setToast(QCoreApplication::translate("OpenAppAction", "Failed to start app"), ToastStyle::Danger);
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
    toast->failure(tr("Failed to start app"));
    return;
  }

  ctx->navigation->closeWindow();
  if (m_clearSearch) ctx->navigation->clearSearchText();
}

OpenRawProgramAction::OpenRawProgramAction(const std::vector<QString> &args) : m_args(args) {}

QuitAppAction::QuitAppAction(const std::shared_ptr<AbstractApplication> &app)
    : AbstractAction(tr("Quit Application"), BuiltinIcon::XMarkCircle), m_app(app) {
  setAutoClose();
}

void QuitAppAction::execute(ApplicationContext *ctx) {
  auto toast = ctx->services->toastService();

  if (!ctx->services->appRuntime()->quit(*m_app)) {
    toast->failure(tr("Failed to quit %1").arg(m_app->displayName()));
    return;
  }

  ctx->navigation->showHud(tr("Quit %1").arg(m_app->displayName()));
}

ForceQuitAppAction::ForceQuitAppAction(const std::shared_ptr<AbstractApplication> &app)
    : AbstractAction(tr("Force Quit Application"), BuiltinIcon::XMarkCircle), m_app(app) {
  setAutoClose();
}

void ForceQuitAppAction::execute(ApplicationContext *ctx) {
  auto toast = ctx->services->toastService();

  if (!ctx->services->appRuntime()->forceQuit(*m_app)) {
    toast->failure(tr("Failed to force quit %1").arg(m_app->displayName()));
    return;
  }

  ctx->navigation->showHud(tr("Force quit %1").arg(m_app->displayName()));
}

UninstallAppAction::UninstallAppAction(const std::shared_ptr<AbstractApplication> &app)
    : AbstractAction(tr("Uninstall Application"), BuiltinIcon::Trash), m_app(app) {
  setStyle(Style::Danger);
}

namespace {

void trashApp(ServiceRegistry *services, NavigationController *navigation,
              const std::shared_ptr<AbstractApplication> &app) {
  if (!services->appDb()->uninstall(*app)) {
    services->toastService()->failure(
        QCoreApplication::translate("UninstallAppAction", "Failed to uninstall %1").arg(app->displayName()));
    return;
  }

  navigation->showHud(
      QCoreApplication::translate("UninstallAppAction", "Uninstalled %1").arg(app->displayName()));
}

void quitThenTrashApp(ServiceRegistry *services, NavigationController *navigation,
                      const std::shared_ptr<AbstractApplication> &app) {
  auto *runtime = services->appRuntime();

  if (!runtime->quit(*app)) {
    services->toastService()->failure(
        QCoreApplication::translate("UninstallAppAction", "Failed to quit %1").arg(app->displayName()));
    return;
  }

  auto *guard = new QObject(runtime);

  QObject::connect(runtime, &AppRuntime::runningAppsChanged, guard, [=]() {
    if (runtime->isRunning(*app)) return;
    guard->deleteLater();
    trashApp(services, navigation, app);
  });

  QTimer::singleShot(std::chrono::seconds(15), guard, [=]() {
    guard->deleteLater();
    services->toastService()->failure(
        QCoreApplication::translate("UninstallAppAction", "%1 did not quit, uninstall cancelled")
            .arg(app->displayName()));
  });
}

} // namespace

void UninstallAppAction::execute(ApplicationContext *ctx) {
  auto app = m_app;
  auto *services = ctx->services;
  auto *navigation = ctx->navigation.get();
  bool const running = services->appRuntime()->isRunning(*app);
  QString const message = running ? tr("%1 is running. It will be quit and moved to the trash.")
                                  : tr("The application will be moved to the trash.");

  navigation->confirmAlert(tr("Uninstall %1?").arg(app->displayName()), message.arg(app->displayName()),
                           [=]() {
                             if (services->appRuntime()->isRunning(*app)) {
                               quitThenTrashApp(services, navigation, app);
                             } else {
                               trashApp(services, navigation, app);
                             }
                           });
}

void OpenInBrowserAction::execute(ApplicationContext *ctx) {
  const auto toast = ctx->services->toastService();

  if (!ctx->services->appDb()->openTarget(m_url)) {
    toast->failure(tr("Failed to open in browser"));
    return;
  }

  ctx->navigation->showHud(tr("Opened in browser"));
}

OpenWithAction::OpenWithAction(QString target)
    : ListSubmenuAction(QCoreApplication::translate("OpenWithAction", "Open with..."), BuiltinIcon::ArrowUp),
      m_target(std::move(target)) {
  setShortcut(Keybind::OpenAction);
}

void OpenWithAction::setTypeFiltering(bool filter) { m_typeFiltered = filter; }

std::unique_ptr<ActionPanelState> OpenWithAction::buildState(ApplicationContext *ctx) const {
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  const auto db = ctx->services->appDb();
  const auto getOpeners = [&]() {
    if (m_typeFiltered) return db->findOpeners(m_target);
    return db->list() | std::views::filter([](auto &&app) { return app->isOpener(); }) |
           std::ranges::to<std::vector>();
  };

  for (const auto &opener : getOpeners()) {
    auto action = new OpenAppAction(opener, opener->displayName(), {m_target});
    section->addAction(action);
  }

  return panel;
}
