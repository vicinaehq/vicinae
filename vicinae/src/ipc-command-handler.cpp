#include "ipc-command-handler.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <QDebug>
#include "services/oauth/oauth-service.hpp"
#include "theme/theme-db.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/config/config-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "services/app-service/app-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <qapplication.h>
#include <qobjectdefs.h>
#include "extension/manager/extension-manager.hpp"
#include <qsqlquery.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/dmenu-view/dmenu-view.hpp"
#include "ui/toast/toast.hpp"
#include "vicinae.hpp"

PromiseLike<proto::ext::daemon::Response *>
IpcCommandHandler::handleCommand(const proto::ext::daemon::Request &request) {
  auto res = new proto::ext::daemon::Response;
  auto &nav = m_ctx.navigation;

  using Req = proto::ext::daemon::Request;

  switch (request.payload_case()) {
  case Req::kPing:
    res->set_allocated_ping(new proto::ext::daemon::PingResponse());
    break;
  case Req::kUrl: {
    auto verbRes = handleUrl(QUrl(request.url().url().c_str()));
    auto urlRes = new proto::ext::daemon::UrlResponse();
    if (!verbRes) { urlRes->set_error(verbRes.error()); }
    res->set_allocated_url(urlRes);
    return res;
  }
  case Req::kDmenu:
    return processDmenu(request.dmenu());
  case Req::kLaunchApp:
    return launchApp(request.launch_app());
  case Req::kListApps:
    return listApps(request.list_apps());
  default:
    break;
  }

  return res;
}

proto::ext::daemon::Response *
IpcCommandHandler::launchApp(const proto::ext::daemon::LaunchAppRequest &request) {
  auto res = new proto::ext::daemon::Response;
  auto launchAppRes = new proto::ext::daemon::LaunchAppResponse;
  auto appDb = m_ctx.services->appDb();
  auto wm = m_ctx.services->windowManager();
  auto app = appDb->findById(request.app_id().c_str());

  res->set_allocated_launch_app(launchAppRes);

  if (!app) {
    launchAppRes->set_error("No app with id " + request.app_id());
    return res;
  }

  if (!request.new_instance()) {
    if (auto wins = wm->findAppWindows(*app); !wins.empty()) {
      auto &win = wins.front();
      wm->provider()->focusWindowSync(*win);
      launchAppRes->set_focused_window_title(win->title().toStdString());
      return res;
    }
  }

  std::vector<QString> args;

  args.reserve(request.args().size());
  for (const auto &arg : request.args()) {
    args.emplace_back(arg.c_str());
  }

  if (!appDb->launch(*app, args)) {
    launchAppRes->set_error("Failed to launch app with id " + request.app_id());
  }

  return res;
}

proto::ext::daemon::Response *
IpcCommandHandler::listApps(const proto::ext::daemon::ListAppsRequest &request) {
  auto res = new proto::ext::daemon::Response;
  auto listAppsRes = new proto::ext::daemon::ListAppsResponse;
  auto appDb = m_ctx.services->appDb();

  res->set_allocated_list_apps(listAppsRes);

  auto apps = appDb->list();

  for (const auto &app : apps) {
    if (app->isAction() && !request.with_actions()) { continue; }

    auto appInfo = listAppsRes->add_apps();
    appInfo->set_id(app->id().toStdString());
    appInfo->set_name(app->displayName().toStdString());
    appInfo->set_hidden(!app->displayable());
    appInfo->set_path(app->path().string());
    appInfo->set_description(app->description().toStdString());
    appInfo->set_program(app->program().toStdString());
    appInfo->set_is_terminal_app(app->isTerminalApp());
    appInfo->set_icon_url(app->iconUrl().name().toStdString());
    appInfo->set_is_action(app->isAction());

    for (const auto &keyword : app->keywords()) {
      appInfo->add_keywords(keyword.toStdString());
    }
  }

  return res;
}

QFuture<proto::ext::daemon::Response *>
IpcCommandHandler::processDmenu(const proto::ext::daemon::DmenuRequest &request) {
  using Watcher = QFutureWatcher<proto::ext::daemon::Response *>;
  auto &nav = m_ctx.navigation;
  QPromise<proto::ext::daemon::Response *> promise;
  auto future = promise.future();
  auto view = new DMenu::View(DMenu::Payload::fromProto(request));
  auto watcher = new Watcher;

  watcher->setFuture(future);

  QObject::connect(watcher, &Watcher::canceled, [nav = nav.get()]() { nav->closeWindow(); });
  QObject::connect(watcher, &Watcher::finished, [watcher]() { watcher->deleteLater(); });
  QObject::connect(view, &DMenu::View::selected, [promise = std::move(promise)](const QString &text) mutable {
    auto dmenuRes = new proto::ext::daemon::DmenuResponse;
    auto res = new proto::ext::daemon::Response;
    res->set_allocated_dmenu(dmenuRes);
    dmenuRes->set_output(text.toStdString());
    promise.addResult(res);
    promise.finish();
  });

  nav->popToRoot({.clearSearch = false});
  nav->pushView(view);
  nav->setInstantDismiss(true);
  nav->showWindow();

  return future;
}

tl::expected<void, std::string> IpcCommandHandler::handleUrl(const QUrl &url) {
  if (!Omnicast::APP_SCHEMES.contains(url.scheme())) {
    return tl::unexpected("Unsupported url scheme " + url.scheme().toStdString());
  }

  qDebug() << "goot deeplink" << url.toString();

  QUrlQuery query(url.query(QUrl::FullyDecoded));

  // Command may be in host (raycast://) or as first part of path (com.raycast:/)
  QString command = url.host();
  QString path = url.path();
  if (command.isEmpty() && !path.isEmpty()) {
    QStringList pathParts = path.split('/', Qt::SkipEmptyParts);
    command = pathParts.at(0);
    path = "/" + pathParts.mid(1).join('/');
  }

  if (command == "ping") { return {}; }

  if (command == "toggle") {
    m_ctx.navigation->toggleWindow();
    if (query.hasQueryItem("fallbackText")) {
      m_ctx.navigation->setSearchText(query.queryItemValue("fallbackText"));
    }
    return {};
  }

  if (command == "settings") {
    if (path == "/open") {
      m_ctx.settings->openWindow();

      if (auto text = query.queryItemValue("tab"); !text.isEmpty()) { m_ctx.settings->openTab(text); }

      return {};
    }
    if (path == "/close") {
      m_ctx.settings->closeWindow();
      return {};
    }
  }

  if (command == "close") {
    if (!m_ctx.navigation->isWindowOpened()) return tl::unexpected("Already closed");

    CloseWindowOptions opts;

    if (auto text = query.queryItemValue("popToRootType"); !text.isEmpty()) {
      if (text == "immediate") { opts.popToRootType = PopToRootType::Immediate; }
      if (text == "suspended") { opts.popToRootType = PopToRootType::Suspended; }
    }

    if (auto text = query.queryItemValue("clearRootSearch"); !text.isEmpty()) {
      opts.clearRootSearch = text == "true" || text == "1";
    }

    m_ctx.navigation->closeWindow(opts);
    return {};
  }

  if (command == "open") {
    if (m_ctx.navigation->isWindowOpened()) return tl::unexpected("Already opened");
    if (auto text = query.queryItemValue("popToRoot"); text == "true" || text == "1") {
      m_ctx.navigation->popToRoot();
    }

    m_ctx.navigation->showWindow();

    if (query.hasQueryItem("fallbackText")) {
      m_ctx.navigation->setSearchText(query.queryItemValue("fallbackText"));
    }

    return {};
  }

  if (command == "pop_current") {
    m_ctx.navigation->popCurrentView();
    return {};
  }

  if (command == "pop_to_root") {
    PopToRootOptions opts;

    if (auto text = query.queryItemValue("clearSearch"); !text.isEmpty()) {
      opts.clearSearch = text == "true" || text == "1";
    }

    m_ctx.navigation->popToRoot(opts);
    return {};
  }

  if (command == "toast") {
    QString title = query.hasQueryItem("title") ? query.queryItemValue("title") : "Toast";
    m_ctx.services->toastService()->setToast(title, ToastStyle::Info);
    return {};
  }

  if (command == "extensions") {
    auto root = m_ctx.services->rootItemManager();

    auto components = path.sliced(1).split('/');

    if (components.size() < 3) {
      qWarning() << "Invalid use of extensions verb: expected format is "
                    "vicinae://extensions/<author>/<ext_name>/<cmd_name>";
      return {};
    }

    QString author = components[0];
    QString extName = components[1];
    QString cmdName = components[2];

    for (ExtensionRootProvider *ext : root->extensions()) {
      for (const auto &cmd : ext->repository()->commands()) {
        // Author is suffixed, check author with suffix
        if (author.contains("@")) {
          if (cmd->authorSuffixed() != author) continue;
        } else {
          // otherwise check plain author name
          if (cmd->author() != author) continue;
        }

        if (cmd->commandId() == cmdName && cmd->repositoryName() == extName) {
          m_ctx.navigation->popToRoot({.clearSearch = false});

          // Read `arguments` query parameter
          ArgumentValues arguments;
          if (query.hasQueryItem("arguments")) {
            QString argsText = query.queryItemValue("arguments");
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(argsText.toUtf8(), &parseError);
            if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
              QJsonObject obj = doc.object();
              for (auto it = obj.begin(); it != obj.end(); ++it) {
                arguments.emplace_back(it.key(), it.value().toString());
              }
            } else {
              qWarning() << "Failed to parse arguments JSON:" << parseError.errorString();
            }
          }

          m_ctx.navigation->launch(cmd, arguments);

          if (auto text = query.queryItemValue("fallbackText"); !text.isEmpty()) {
            m_ctx.navigation->setSearchText(text);
          }

          if (cmd->isView() && !m_ctx.navigation->isWindowOpened()) {
            m_ctx.navigation->setInstantDismiss();
            m_ctx.navigation->showWindow();
          }

          break;
        }
      }
    }

    return {};
  }

  if (command == "theme") {
    auto components = path.sliced(1).split('/');
    auto verb = components.at(0);

    if (verb == "set") {
      if (components.size() != 2) {
        return tl::unexpected("Correct usage is vicinae://theme/set/<theme_id>");
      }

      QString id = components.at(1);
      auto &service = ThemeService::instance();
      auto cfg = m_ctx.services->config();

      service.db().scan();

      auto theme = service.findTheme(id);

      if (!theme) {
        return tl::unexpected(std::string("theme with id ") + id.toStdString() + " does not exist");
      }

      if (theme->id() == cfg->value().theme.name.value_or("")) {
        service.reloadCurrentTheme();
      } else {
        cfg->updateConfig([&](ConfigService::Value &value) { value.theme.name = theme->id(); });
      }

      if (auto text = query.queryItemValue("openWindow"); text == "true" || text == "1") {
        m_ctx.navigation->showWindow();
      }

      return {};
    }
  }

  if (command == "oauth") {
    auto oauth = m_ctx.services->oauthService();
    QString code = query.queryItemValue("code");
    QString state = query.queryItemValue("state");
    oauth->fullfillRequest(state, code);
    return {};
  }

  if (command == "api") {
    auto registry = m_ctx.services->extensionRegistry();
    auto id = query.queryItemValue("id");

    if (id.isEmpty()) {
      qWarning() << "Missing valid extension id from URI";
      return {};
    }

    if (path == "/extensions/develop/start") {
      m_ctx.services->extensionManager()->addDevelopmentSession(id);

      qInfo() << "Start extension development session for" << id;
      // the caller should have created or updated a new extension bundle at that point
      // so all we have to do is to rescan.
      // this hook is how we can know to launch an extension in development mode instead of production
      registry->requestScan();
      return {};
    }

    if (path == "/extensions/develop/refresh") {
      qInfo() << "Refreshing extension development for" << id;

      // we just rescan all bundles, we don't really need to do it incrementally for now
      // an extension is "hot reloaded" although state is not preserved (this is a very tricky thing to
      // implement properly)
      registry->requestScan();

      if (auto cmd = m_ctx.navigation->activeCommand(); cmd && cmd->extensionId() == id) {
        qInfo() << "Reloading active command following extension refresh";
        m_ctx.navigation->reloadActiveCommand();
      }

      return {};
    }

    if (path == "/extensions/develop/stop") {
      m_ctx.services->extensionManager()->removeDevelopmentSession(id);
      qInfo() << "Stopping extension development for" << id;
      // stopping a development session doesn't remove the bundle, but if a command
      // from the extension is launched outside of dev mode it's going to be run in
      // the production environment (although the bundle itself won't be optimized for production)
      return {};
    }
  }

  if (command == "internal") {
    if (path == "/restart-extension-runtime") {
      qInfo() << "Restarting extension runtime....";
      m_ctx.navigation->popToRoot();
      m_ctx.services->extensionManager()->start();
      return {};
    }
  }

  return tl::unexpected(std::string("invalid deeplink ") + url.toString().toStdString());
}

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
