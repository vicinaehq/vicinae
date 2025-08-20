#include "ipc-command-handler.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <algorithm>
#include "services/toast/toast-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <qlogging.h>
#include <qobjectdefs.h>
#include "extension/manager/extension-manager.hpp"
#include <qsqlquery.h>
#include <qurl.h>
#include <qurlquery.h>
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "omni-command-db.hpp"
#include "command-controller.hpp"
#include "theme.hpp"
#include "ui/toast/toast.hpp"
#include "vicinae.hpp"

proto::ext::daemon::Response *IpcCommandHandler::handleCommand(const proto::ext::daemon::Request &request) {
  auto res = new proto::ext::daemon::Response;
  auto &nav = m_ctx.navigation;

  switch (request.payload_case()) {
  case proto::ext::daemon::Request::kUrl: {
    handleUrl(QUrl(request.url().url().c_str()));
    res->set_allocated_url(new proto::ext::daemon::UrlResponse());
    break;
  }
  default:
    break;
  }

  return res;
}

void IpcCommandHandler::handleUrl(const QUrl &url) {
  if (!std::ranges::contains(Omnicast::APP_SCHEMES, url.scheme())) {
    qWarning() << "Unsupported url scheme" << url.scheme() << "Supported schemes are"
               << Omnicast::APP_SCHEMES;
    return;
  }

  QUrlQuery query(url.query());

  if (url.host() == "ping") { return; }

  if (url.host() == "toggle") {
    m_ctx.navigation->toggleWindow();
    return;
  }

  if (url.host() == "settings") {
    if (url.path() == "/open") {
      m_ctx.settings->openWindow();

      if (auto text = query.queryItemValue("tab"); !text.isEmpty()) { m_ctx.settings->openTab(text); }

      return;
    }
    if (url.path() == "/close") {
      m_ctx.settings->closeWindow();
      return;
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
    return;
  }

  if (url.host() == "open") {
    m_ctx.navigation->showWindow();
    return;
  }

  if (url.host() == "pop_current") {
    m_ctx.navigation->popCurrentView();
    return;
  }

  if (url.host() == "pop_to_root") {
    PopToRootOptions opts;

    if (auto text = query.queryItemValue("clearSearch"); !text.isEmpty()) {
      opts.clearSearch = text == "true" || text == "1";
    }

    m_ctx.navigation->popToRoot(opts);
    return;
  }

  if (url.host() == "toast") {
    QString title = query.hasQueryItem("title") ? query.queryItemValue("title") : "Toast";
    m_ctx.services->toastService()->setToast(title, ToastPriority::Info);
    return;
  }

  if (url.host() == "extensions") {
    auto components = url.path().sliced(1).split('/');

    if (components.size() < 3) {
      qWarning() << "Invalid use of extensions verb: expected format is "
                    "vicinae://extensions/<author>/<ext_name>/<cmd_name>";
      return;
    }

    QString author = components[0];
    QString extName = components[1];
    QString cmdName = components[2];

    for (const auto &cmd : m_ctx.services->commandDb()->commands()) {
      if (cmd.command->author() == author && cmd.command->commandId() == cmdName &&
          cmd.command->repositoryName() == extName) {
        m_ctx.command->launch(cmd.command);

        if (auto text = query.queryItemValue("fallbackText"); !text.isEmpty()) {
          m_ctx.navigation->setSearchText(text);
        }

        m_ctx.navigation->showWindow();

        break;
      }
    }

    return;
  }

  if (url.host() == "theme") {
    auto components = url.path().sliced(1).split('/');
    auto verb = components.at(0);

    if (verb == "set") {
      if (components.size() != 2) {
        qCritical() << "Correct usage is vicinae://theme/set/<theme_id>";
        return;
      }

      QString id = components.at(1);

      if (!ThemeService::instance().setTheme(id)) {
        qWarning() << "Failed to set theme with id" << id << "(this theme most likely doesn't exist)";
        return;
      }

      return;
    }
  }

  if (url.host() == "api") {
    auto registry = m_ctx.services->extensionRegistry();
    auto id = query.queryItemValue("id");

    if (id.isEmpty()) {
      qWarning() << "Missing valid extension id from URI";
      return;
    }

    if (url.path() == "/extensions/develop/start") {
      m_ctx.services->extensionManager()->addDevelopmentSession(id);

      qInfo() << "Start extension development session for" << id;
      // the caller should have created or updated a new extension bundle at that point
      // so all we have to do is to rescan.
      // this hook is how we can know to launch an extension in development mode instead of production
      registry->requestScan();
      return;
    }

    if (url.path() == "/extensions/develop/refresh") {
      qInfo() << "Refreshing extension development for" << id;

      // we just rescan all bundles, we don't really need to do it incrementally for now
      // an extension is "hot reloaded" although state is not preserved (this is a very tricky thing to
      // implement properly)
      registry->requestScan();

      if (auto cmd = m_ctx.command->activeCommand(); cmd && cmd->extensionId() == id) {
        qInfo() << "Reloading active command following extension refresh";
        m_ctx.command->reloadActiveCommand();
      }

      return;
    }

    if (url.path() == "/extensions/develop/stop") {
      m_ctx.services->extensionManager()->removeDevelopmentSession(id);
      qInfo() << "Stopping extension development for" << id;
      // stopping a development session doesn't remove the bundle, but if a command
      // from the extension is launched outside of dev mode it's going to be run in
      // the production environment (although the bundle itself won't be optimized for production)
      return;
    }
  }

  qWarning() << "No handler for URL" << url;
}

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
