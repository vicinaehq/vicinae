#include "ipc-command-handler.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <algorithm>
#include "services/toast/toast-service.hpp"
#include <qlogging.h>
#include <qobjectdefs.h>
#include <qsqlquery.h>
#include <qurl.h>
#include <qurlquery.h>
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "omni-command-db.hpp"
#include "command-controller.hpp"
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

  if (url.host() == "toggle") {
    m_ctx.navigation->toggleWindow();
    return;
  }

  if (url.host() == "close") {
    m_ctx.navigation->closeWindow();
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
    m_ctx.navigation->popToRoot();
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

  qWarning() << "No handler for URL" << url;
}

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
