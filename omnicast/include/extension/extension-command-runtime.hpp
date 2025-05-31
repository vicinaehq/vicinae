#pragma once
#include "base-view.hpp"
#include "command.hpp"
#include "common.hpp"
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/model-parser.hpp"
#include "extend/root-detail-model.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-list-component.hpp"
#include "extension/extension-view.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "service-registry.hpp"
#include <algorithm>
#include <memory>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qwidget.h>
#include <variant>

struct ExtensionViewInfo {
  size_t index;
  ExtensionSimpleView *view;
};

struct ViewVisitor {
  ExtensionSimpleView *operator()(const ListModel &model) const { return new ExtensionListComponent; }
  ExtensionSimpleView *operator()(const GridModel &model) const { return nullptr; }
  ExtensionSimpleView *operator()(const FormModel &model) const { return nullptr; }
  ExtensionSimpleView *operator()(const InvalidModel &model) const { return nullptr; }
  ExtensionSimpleView *operator()(const RootDetailModel &model) const { return nullptr; }
};

class PlaceholderExtensionView : public SimpleView {
public:
  PlaceholderExtensionView() {
    m_topBar->input->hide();
    setupUI(new QWidget);
  }
};

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<ExtensionViewInfo> m_viewStack;
  QFutureWatcher<std::vector<RenderModel>> m_modelWatcher;
  PlaceholderExtensionView *placeholderView = new PlaceholderExtensionView;

  QString m_sessionId;

  void sendEvent(const QString &handlerId, const QJsonArray &args) {
    auto manager = ServiceRegistry::instance()->extensionManager();
    QJsonObject payload;

    payload["args"] = args;
    manager->emitExtensionEvent(m_sessionId, handlerId, payload);
  }

  ExtensionSimpleView *createViewFromModel(const RenderModel &model) {
    auto view = std::visit(ViewVisitor(), model);

    view->setNavigationTitle(m_command->name());
    view->setNavigationIcon(m_command->iconUrl());
    connect(view, &ExtensionSimpleView::notificationRequested, this, &ExtensionCommandRuntime::sendEvent);

    return view;
  }

  QJsonObject handleStorage(const QString &action, const QJsonObject &payload) {
    auto storage = ServiceRegistry::instance()->localStorage();

    if (action == "storage.clear") {
      storage->clearNamespace(m_command->extensionId());

      return {};
    }

    if (action == "storage.get") {
      auto key = payload.value("key").toString();
      QJsonValue value = storage->getItem(m_command->extensionId(), key);
      QJsonObject response;

      response["value"] = value;

      return response;
    }

    if (action == "storage.set") {
      auto key = payload.value("key").toString();
      auto value = payload.value("value");

      storage->setItem(m_command->extensionId(), key, value);

      return {};
    }

    if (action == "storage.remove") {
      auto key = payload.value("key").toString();

      storage->removeItem(m_command->extensionId(), key);
      return {};
    }

    if (action == "storage.list") {
      QJsonObject response;

      response["values"] = storage->listNamespaceItems(m_command->extensionId());

      return response;
    }

    return {};
  }

  QJsonObject handleAI(const QString &action, const QJsonObject &payload) {
    auto manager = ServiceRegistry::instance()->AI();

    if (action == "ai.get-models") {
      QJsonArray items;

      for (const auto &model : manager->listModels()) {
        if (!(model.capabilities & AiModel::Capability::Reasoning)) continue;

        QJsonObject obj;

        obj["id"] = model.id;
        obj["name"] = model.displayName;
        obj["icon"] = model.iconName ? *model.iconName : QJsonValue();

        items.push_back(obj);
      }

      return {{"models", items}};
    }

    if (action == "ai.create-completion") {
      auto prompt = payload.value("prompt").toString();
      auto callback = payload.value("callback").toString();
      auto completion = manager->createCompletion(prompt);

      connect(completion, &StreamedChatCompletion::tokenReady, this,
              [this, callback](const ChatCompletionToken &token) {
                QJsonObject payload;

                payload["token"] = token.message.content().toString();
                payload["done"] = false;

                sendEvent(callback, {payload});
              });
      connect(completion, &StreamedChatCompletion::finished, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        sendEvent(callback, {payload});
        completion->deleteLater();
      });
      connect(completion, &StreamedChatCompletion::errorOccured, this, [this, completion, callback]() {
        QJsonObject payload;

        payload["token"] = "";
        payload["done"] = true;
        sendEvent(callback, {payload});
        completion->deleteLater();
      });

      return {{"started", true}};
    }

    return {};
  }

  QJsonObject handleAppRequest(const QString &action, const QJsonObject &payload) {
    auto appDb = ServiceRegistry::instance()->appDb();

    if (action == "apps.list") {
      auto baseApps = appDb->list();
      QJsonArray apps;

      auto serializedApps = baseApps |
                            std::views::filter([](const auto &app) { return app->displayable(); }) |
                            std::views::transform([](const auto &app) {
                              QJsonObject appObj;

                              appObj["id"] = app->id();
                              appObj["name"] = app->name();
                              appObj["icon"] = app->iconUrl().name();

                              return appObj;
                            });

      std::ranges::for_each(serializedApps, [&apps](const auto &a) { apps.push_back(a); });

      return {{"apps", apps}};
    }

    if (action == "apps.open") {
      auto target = payload.value("target").toString();

      if (payload.contains("appId")) {
        auto appId = payload.value("appId").toString();

        if (auto app = appDb->findById(appId)) {
          if (appDb->launch(*app, {target})) return {};
        }
      }

      if (auto opener = appDb->findBestOpener(target)) { appDb->launch(*opener, {target}); }

      return {};
    }

    return {};
  }

  QJsonObject handleToastRequest(const QString &action, const QJsonObject &payload) {
    auto ui = ServiceRegistry::instance()->UI();

    if (action == "toast.show") {
      auto title = payload["title"].toString();
      auto style = payload["style"].toString();
      ToastPriority priority;

      if (style == "success") {
        priority = ToastPriority::Success;
      } else if (style == "failure") {
        priority = ToastPriority::Danger;
      }

      ui->setToast(title, priority);
      return {};
    }

    if (action == "toast.hide") {
      // XXX - Implement actual toast hide
      return {};
    }

    if (action == "toast.update") {
      // XXX - Implement actual toast update
      return {};
    }

    return {};
  }

  QJsonObject handleClipboard(const QString &action, const QJsonObject &payload) {
    auto clipman = ServiceRegistry::instance()->clipman();

    if (action == "clipboard.copy") {
      auto content = Clipboard::fromJson(payload.value("content").toObject());
      auto options = payload.value("options").toObject();
      Clipboard::CopyOptions copyActions;

      copyActions.concealed = options.value("concealed").toBool(false);
      clipman->copyContent(content, copyActions);
      return {};
    }

    return {};
  }

  QJsonObject handleUI(const QString &action, const QJsonObject &payload) {
    auto ui = ServiceRegistry::instance()->UI();

    if (m_command->isView() && action == "ui.push-view") {
      // we create a new view on render
      return {};
    }

    if (m_command->isView() && action == "ui.pop-view") {
      handlePopViewRequest();
      return {};
    }

    if (action == "ui.show-hud") {
      ui->popToRoot();
      ui->closeWindow();
      return {};
    }

    if (action == "ui.close-main-window") {
      ui->closeWindow();
      return {};
    }

    if (action == "ui.clear-search-bar") {
      // m_app->topBar->input->clear();
      // emit m_app->topBar->input->textEdited("");
      return {};
    }

    return {};
  }

  void handlePopViewRequest() {
    auto ui = ServiceRegistry::instance()->UI();

    m_viewStack.pop_back();
    ui->popView();
  }

  void modelCreated() {
    if (m_modelWatcher.isCanceled()) return;

    auto ui = ServiceRegistry::instance()->UI();

    auto models = m_modelWatcher.result();
    qCritical() << "RENDER EXTENSION!!";

    for (int i = 0; i != models.size(); ++i) {
      auto model = models.at(i);

      if (i >= m_viewStack.size()) {
        auto next = createViewFromModel(model);
        if (ui->topView() == placeholderView) {
          ui->replaceView(placeholderView, next);
        } else {
          ui->pushView(next);
        }
        m_viewStack.push_back({.index = model.index(), .view = next});
      } else {
        auto &view = m_viewStack.at(i);

        if (view.index != model.index()) {
          auto next = createViewFromModel(model);

          ui->replaceView(view.view, next);
          view.view = next;
          view.index = model.index();
        }
      }

      m_viewStack.at(i).view->render(model);
    }
  }

  void handleRender(const QJsonArray &views) {
    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([views]() { return ModelParser().parse(views); }));
  }

  void handleRequest(const QString &sessionId, const QString &requestId, const QString &action,
                     const QJsonObject &payload) {
    qDebug() << "request" << action;
    auto manager = ServiceRegistry::instance()->extensionManager();

    if (sessionId != m_sessionId) return;

    qDebug() << "extension request" << requestId;

    if (action.startsWith("storage.")) {
      manager->respond(requestId, handleStorage(action, payload));
      return;
    }

    if (action.startsWith("ai.")) {
      manager->respond(requestId, handleAI(action, payload));
      return;
    }

    if (action.startsWith("apps.")) {
      manager->respond(requestId, handleAppRequest(action, payload));
      return;
    }

    if (action.startsWith("toast.")) {
      manager->respond(requestId, handleToastRequest(action, payload));
      return;
    }

    if (action.startsWith("clipboard.")) {
      manager->respond(requestId, handleClipboard(action, payload));
      return;
    }

    if (action.startsWith("ui.")) {
      manager->respond(requestId, handleUI(action, payload));
      return;
    }
  }

  void handleEvent(const QString &sessionId, const QString &action, const QJsonObject &payload) {
    qDebug() << "event" << action << "for " << sessionId;
    if (m_sessionId != sessionId) return;

    if (action == "unload") {}

    if (action == "ui.render") {
      auto views = payload.value("views").toArray();
      QJsonDocument doc;

      doc.setObject(payload);

      // std::cout << doc.toJson().toStdString();

      return handleRender(views);
    }
  }

  void commandLoaded(const LoadedCommand &command) { m_sessionId = command.sessionId; }

public:
  void load(const LaunchProps &props) override {
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto preferenceValues = commandDb->getPreferenceValues(m_command->uniqueId());
    auto manager = ServiceRegistry::instance()->extensionManager();
    auto ui = ServiceRegistry::instance()->UI();

    if (m_command->mode() == CommandModeView) {
      // We push the first view immediately, waiting for the initial render to come
      // in and "hydrate" it.
      placeholderView->setNavigationTitle(m_command->name());
      placeholderView->setNavigationIcon(m_command->iconUrl());
      ui->pushView(placeholderView);

      connect(ui, &UIController::popViewRequested, this, [this]() {
        m_viewStack.pop_back();

        if (!m_viewStack.empty()) { sendEvent("pop-view", {}); }
      });
    }

    manager->loadCommand(m_command->extensionId(), m_command->commandId(), preferenceValues, props);
  }

  void unload() override {
    auto manager = ServiceRegistry::instance()->extensionManager();

    manager->unloadCommand(m_sessionId);
  }

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command)
      : CommandContext(command), m_command(command) {
    auto manager = ServiceRegistry::instance()->extensionManager();

    connect(manager, &ExtensionManager::extensionRequest, this, &ExtensionCommandRuntime::handleRequest);
    connect(manager, &ExtensionManager::extensionEvent, this, &ExtensionCommandRuntime::handleEvent);
    connect(manager, &ExtensionManager::commandLoaded, this, &ExtensionCommandRuntime::commandLoaded);
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this,
            &ExtensionCommandRuntime::modelCreated);
  }
};
