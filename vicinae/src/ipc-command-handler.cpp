#include "ipc-command-handler.hpp"
#include "common.hpp"
#include "proto/daemon.pb.h"
#include <qlogging.h>
#include "navigation-controller.hpp"

proto::ext::daemon::Response *IpcCommandHandler::handleCommand(const proto::ext::daemon::Request &request) {
  auto res = new proto::ext::daemon::Response;
  auto &nav = m_ctx.navigation;

  qDebug() << "Got ipc command";

  switch (request.payload_case()) {
  case proto::ext::daemon::Request::kPing: {
    qDebug() << "Got answering ping";
    res->set_allocated_ping(new proto::ext::daemon::PingResponse());
    break;
  }
  case proto::ext::daemon::Request::kToggle: {
    nav->toggleWindow();
    res->set_allocated_toggle(new proto::ext::daemon::ToggleResponse());
    break;
  }
  case proto::ext::daemon::Request::kUrl: {
    qDebug() << "GOT URL" << request.url().url();
    res->set_allocated_url(new proto::ext::daemon::UrlResponse());
    break;
  }
  default:
    break;
  }

  return res;
}

/*
std::variant<CommandResponse, CommandError> IpcCommandHandler::handleCommand(const CommandMessage &message) {
  qDebug() << "received message type" << message.type;
  auto commandDb = m_ctx.services->commandDb();
  auto extensionManager = m_ctx.services->extensionManager();
  auto &nav = m_ctx.navigation;

  if (message.type == "ping") { return "pong"; }
  if (message.type == "toggle") {
    nav->toggleWindow();
    return true;
  }

  if (message.type == "url-scheme-handler") {
    QUrl url(message.params.asString().c_str());

    qCritical() << "Got url" << url;

    // omnicast://extensions/<extension_id>/<command_id>
    if (url.host() == "extensions") {
      auto ss = url.path().slice(1).split('/');
      auto extId = ss.at(0);
      auto commandId = ss.at(1);

      if (ss.size() < 2) {
        qCritical() << "Malformed extensions request";
        return false;
      }

      for (auto &entry : commandDb->commands()) {
        if (entry.command->extensionId() == extId && entry.command->commandId() == commandId) {
          // ui->launchCommand(entry.command);
          return true;
        }
      }

      qCritical() << "No command id" << extId << commandId;
    }

    if (url.path() == "/api/extensions/develop/start") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      // extensionManager->startDevelopmentSession(id);
      qDebug() << "start develop id" << query.queryItemValue("id");
    }

    else if (url.path() == "/api/extensions/develop/refresh") {
      QUrlQuery query(url.query());
      QString id = query.queryItemValue("id");

      // extensionManager->refreshDevelopmentSession(id);
      qDebug() << "refresh develop id" << id;
    }

    qDebug() << "handling URL in daemon" << url.toString();
    return {};
  }

  if (message.type == "command.list") {
    Proto::Array results;

    for (const auto &entry : commandDb->commands()) {
      Proto::Dict result;

      result["id"] = entry.command->uniqueId().toUtf8().constData();
      result["name"] = entry.command->name().toUtf8().constData();
      results.push_back(result);
    }

    return results;
  }

  if (message.type == "command.push") {
    auto args = message.params.asArray();

    if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

    auto id = args.at(0).asString();

    return CommandError{"No such command"};
  }

  return CommandError{"Unknowm command"};
}
*/

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
