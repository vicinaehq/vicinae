#include "ipc-command-handler.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <QDebug>
#include "theme/theme-db.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "services/config/config-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <qapplication.h>
#include <qobjectdefs.h>
#include "extension/manager/extension-manager.hpp"
#include <qsqlquery.h>
#include <qurl.h>
#include <qurlquery.h>
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
  default:
    break;
  }

  return res;
}

QFuture<proto::ext::daemon::Response *>
IpcCommandHandler::processDmenu(const proto::ext::daemon::DmenuRequest &request) {
  using Watcher = QFutureWatcher<proto::ext::daemon::Response *>;
  auto &nav = m_ctx.navigation;
  QPromise<proto::ext::daemon::Response *> promise;
  auto future = promise.future();
  DMenuListView::DmenuPayload payload;

  payload.raw = request.raw_content();
  payload.placeholder = request.placeholder();
  payload.sectionTitle = request.section_title();
  payload.noSection = request.no_section();
  payload.navigationTitle = request.navigation_title();

  auto view = new DMenuListView(payload);
  auto watcher = new Watcher;

  watcher->setFuture(future);

  QObject::connect(watcher, &Watcher::canceled, [nav = nav.get()]() { nav->closeWindow(); });
  QObject::connect(watcher, &Watcher::finished, [watcher]() { watcher->deleteLater(); });
  QObject::connect(view, &DMenuListView::selected,
                   [promise = std::move(promise)](const QString &text) mutable {
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

  QUrlQuery query(url.query());

  if (url.host() == "ping") { return {}; }

  if (url.host() == "toggle") {
    m_ctx.navigation->toggleWindow();
    return {};
  }

  if (url.host() == "settings") {
    if (url.path() == "/open") {
      m_ctx.settings->openWindow();

      if (auto text = query.queryItemValue("tab"); !text.isEmpty()) { m_ctx.settings->openTab(text); }

      return {};
    }
    if (url.path() == "/close") {
      m_ctx.settings->closeWindow();
      return {};
    }
  }

  if (url.host() == "close") {
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

  if (url.host() == "open") {
    if (auto text = query.queryItemValue("popToRoot"); text == "true" || text == "1") {
      m_ctx.navigation->popToRoot();
    }

    m_ctx.navigation->showWindow();
    return {};
  }

  if (url.host() == "pop_current") {
    m_ctx.navigation->popCurrentView();
    return {};
  }

  if (url.host() == "pop_to_root") {
    PopToRootOptions opts;

    if (auto text = query.queryItemValue("clearSearch"); !text.isEmpty()) {
      opts.clearSearch = text == "true" || text == "1";
    }

    m_ctx.navigation->popToRoot(opts);
    return {};
  }

  if (url.host() == "toast") {
    QString title = query.hasQueryItem("title") ? query.queryItemValue("title") : "Toast";
    m_ctx.services->toastService()->setToast(title, ToastStyle::Info);
    return {};
  }

  if (url.host() == "extensions") {
    auto root = m_ctx.services->rootItemManager();

    auto components = url.path().sliced(1).split('/');

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
        if (cmd->author() == author && cmd->commandId() == cmdName && cmd->repositoryName() == extName) {
          m_ctx.navigation->popToRoot({.clearSearch = false});
          m_ctx.navigation->launch(cmd);

          if (auto text = query.queryItemValue("fallbackText"); !text.isEmpty()) {
            m_ctx.navigation->setSearchText(text);
          }

          if (!m_ctx.navigation->isWindowOpened()) {
            m_ctx.navigation->setInstantDismiss();
            m_ctx.navigation->showWindow();
          }

          break;
        }
      }
    }

    return {};
  }

  if (url.host() == "theme") {
    auto components = url.path().sliced(1).split('/');
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

  if (url.host() == "api") {
    auto registry = m_ctx.services->extensionRegistry();
    auto id = query.queryItemValue("id");

    if (id.isEmpty()) {
      qWarning() << "Missing valid extension id from URI";
      return {};
    }

    if (url.path() == "/extensions/develop/start") {
      m_ctx.services->extensionManager()->addDevelopmentSession(id);

      qInfo() << "Start extension development session for" << id;
      // the caller should have created or updated a new extension bundle at that point
      // so all we have to do is to rescan.
      // this hook is how we can know to launch an extension in development mode instead of production
      registry->requestScan();
      return {};
    }

    if (url.path() == "/extensions/develop/refresh") {
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

    if (url.path() == "/extensions/develop/stop") {
      m_ctx.services->extensionManager()->removeDevelopmentSession(id);
      qInfo() << "Stopping extension development for" << id;
      // stopping a development session doesn't remove the bundle, but if a command
      // from the extension is launched outside of dev mode it's going to be run in
      // the production environment (although the bundle itself won't be optimized for production)
      return {};
    }
  }

  if (url.host() == "internal") {
    if (url.path() == "/restart-extension-runtime") {
      qInfo() << "Restarting extension runtime....";
      m_ctx.navigation->popToRoot();
      m_ctx.services->extensionManager()->start();
      return {};
    }
  }

  return tl::unexpected(std::string("invalid deeplink ") + url.toString().toStdString());
}

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
