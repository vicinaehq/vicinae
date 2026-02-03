#include "ipc-command-handler.hpp"
#include "common.hpp"
#include <QDebug>
#include <QFutureWatcher>
#include "services/oauth/oauth-service.hpp"
#include "theme/theme-db.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <algorithm>
#include <qapplication.h>
#include <qfuturewatcher.h>
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
#include "ui/provider-view/provider-view.hpp"
#include "ui/toast/toast.hpp"
#include "vicinae.hpp"

std::expected<void, std::string> IpcCommandHandler::handleUrl(const QUrl &url) {
  if (!Omnicast::APP_SCHEMES.contains(url.scheme())) {
    return std::unexpected("Unsupported url scheme " + url.scheme().toStdString());
  }

  qDebug() << "got deeplink" << url.toString();

  QUrlQuery query(url.query(QUrl::FullyDecoded));

  // Command may be in host (raycast://) or as first part of path (com.raycast:/)
  QString command = url.host();
  QString path = url.path();
  if (command.isEmpty() && !path.isEmpty()) {
    QStringList pathParts = path.split('/', Qt::SkipEmptyParts);
    command = pathParts.at(0);
    path = "/" + pathParts.mid(1).join('/');
  }

  // TODO: add a "quit" command to handle graceful shutdown (requires more work than you would expect)
  if (command == "kill") {
    qInfo() << "Killing vicinae server because a new instance was started";
    QApplication::exit(1);
    return {};
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
    if (!m_ctx.navigation->isWindowOpened()) return std::unexpected("Already closed");

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
    if (m_ctx.navigation->isWindowOpened()) return std::unexpected("Already opened");
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

    if (components.size() == 2) {
      QString author = components[0];
      QString extName = components[1];
      auto pred = [&](const ExtensionRootProvider *s) {
        return s->repository()->author() == author && s->repository()->name() == extName;
      };

      auto extensions = root->extensions();

      if (auto it = std::ranges::find_if(extensions, pred); it != extensions.end()) {
        m_ctx.navigation->popToRoot({.clearSearch = false});
        m_ctx.navigation->pushView(new ProviderSearchView(**it));

        if (auto text = query.queryItemValue("fallbackText"); !text.isEmpty()) {
          m_ctx.navigation->setSearchText(text);
        }

        if (!m_ctx.navigation->isWindowOpened()) {
          m_ctx.navigation->setInstantDismiss();
          m_ctx.navigation->showWindow();
        }
        return {};
      }

      return std::unexpected("No such extension");
    }

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
        return std::unexpected("Correct usage is vicinae://theme/set/<theme_id>");
      }

      QString id = components.at(1);
      auto &service = ThemeService::instance();
      auto cfg = m_ctx.services->config();

      service.db().scan();

      auto theme = service.findTheme(id);

      if (!theme) {
        return std::unexpected(std::string("theme with id ") + id.toStdString() + " does not exist");
      }

      if (theme->id() == cfg->value().systemTheme().name) {
        service.reloadCurrentTheme();
      } else {
        cfg->mergeThemeConfig({.name = theme->id().toStdString()});
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

  return std::unexpected(std::string("invalid deeplink ") + url.toString().toStdString());
}

IpcCommandHandler::IpcCommandHandler(ApplicationContext &ctx) : m_ctx(ctx) {}
