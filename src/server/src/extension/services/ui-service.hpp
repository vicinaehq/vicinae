#pragma once
#include "extension/extension-command.hpp"
#include "extension/services/tsapi-image.hpp"
#include "generated/tsapi.hpp"
#include "glaze-qt.hpp"
#include "navigation-controller.hpp"
#include "qml/extension-view-host.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert/alert.hpp"
#include <QClipboard>
#include <QGuiApplication>
#include <QtConcurrent/QtConcurrent>
#include <qfuturewatcher.h>
#include <qlogging.h>

class ExtUIService : public tsapi::AbstractUI {
  Q_OBJECT

  using Void = tsapi::Result<void>;

  struct ViewEntry {
    BaseView *baseView;
    std::function<void(const RenderModel &)> renderFn;
  };

public:
  ExtUIService(tsapi::RpcTransport &transport, NavigationController *navigation,
               const std::shared_ptr<ExtensionCommand> &command, tsapi::AbstractEventCore *eventCore,
               ToastService &toast, QObject *parent = nullptr)
      : AbstractUI(transport), m_navigation(navigation), m_command(command), m_eventCore(eventCore),
        m_toast(toast) {
    connect(&m_modelWatcher, &QFutureWatcher<ParsedRenderData>::finished, this, &ExtUIService::modelCreated);
    connect(navigation, &NavigationController::viewPoped, this, &ExtUIService::handleViewPoped);
  }

  void setSubtitleOverride(const std::optional<QString> &subtitle) {
    m_command->setSubtitleOverride(subtitle);
  }

  Void::Future render(std::string json) override {
    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([json = std::move(json)]() -> ParsedRenderData {
      QJsonParseError parseError;
      auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
      if (parseError.error) return {};
      return ModelParser().parse(doc.object().value("views").toArray());
    }));

    return Void::ok();
  }

  Void::Future showToast(std::string id, std::string title, std::string message,
                         tsapi::ToastStyle style) override {
    m_toast.setToast(QString::fromStdString(title), mapToastStyle(style), QString::fromStdString(message));
    return Void::ok();
  }

  Void::Future updateToast(std::string id, std::string title) override { return Void::ok(); }

  Void::Future hideToast(std::string id) override {
    m_toast.clear();
    return Void::ok();
  }

  Void::Future showHud(std::string text, bool clear_root, tsapi::PopToRootType popToRoot) override {
    m_navigation->closeWindow({
        .popToRootType = mapPopToRoot(popToRoot),
        .clearRootSearch = clear_root,
    });
    m_navigation->showHud(QString::fromStdString(text));
    return Void::ok();
  }

  Void::Future closeMainWindow(bool clearRoot, tsapi::PopToRootType popToRoot) override {
    using namespace std::chrono_literals;
    m_navigation->closeWindow({.popToRootType = mapPopToRoot(popToRoot), .clearRootSearch = clearRoot}, 50ms);
    return Void::ok();
  }

  Void::Future popToRoot(bool clearSearchBar) override {
    m_navigation->popToRoot({.clearSearch = clearSearchBar});
    return Void::ok();
  }

  tsapi::Result<bool>::Future confirmAlert(tsapi::ConfirmAlertPayload payload) override {
    auto alert = new CallbackAlertWidget;
    auto promise = std::make_shared<QPromise<tsapi::Result<bool>::Type>>();
    auto future = promise->future();

    promise->start();
    alert->setTitle(QString::fromStdString(payload.title));
    alert->setMessage(QString::fromStdString(payload.description));
    alert->setConfirmText(QString::fromStdString(payload.primaryAction.title), SemanticColor::Red);
    alert->setCancelText(QString::fromStdString(payload.dismissAction.title), SemanticColor::Foreground);

    if (payload.icon) {
      ImageURL url = TsapiImage::parse(*payload.icon);
      if (url.isBuiltin()) { url.setFill(SemanticColor::Red); }
      alert->setIcon(url);
    }

    alert->setCallback([promise](bool value) mutable {
      promise->addResult(tsapi::Result<bool>::Type{value});
      promise->finish();
    });

    m_navigation->setDialog(alert);

    return future;
  }

  Void::Future pushView() override {
    auto notifyFn = makeNotifyFn();
    auto *host = new ExtensionViewHost(notifyFn);

    m_navigation->pushView(host);
    m_navigation->setNavigationTitle(m_command->name());
    m_navigation->setNavigationIcon(m_command->iconUrl());

    m_views.emplace_back(ViewEntry{host, [host](const RenderModel &m) { host->render(m); }});

    QTimer::singleShot(0, this, [this]() { emitviewPushed(); });

    return Void::ok();
  }

  Void::Future popView() override {
    m_navigation->popCurrentView();
    return Void::ok();
  }

  Void::Future setSearchText(std::string text) override {
    m_navigation->setSearchText(QString::fromStdString(text));
    return Void::ok();
  }

  tsapi::Result<std::string>::Future getSelectedText() override {
    auto text = QGuiApplication::clipboard()->text(QClipboard::Mode::Selection);
    return tsapi::Result<std::string>::ok(text.toStdString());
  }

private slots:
  void modelCreated() {
    if (m_modelWatcher.isCanceled()) return;

    auto models = m_modelWatcher.result();

    for (size_t i = 0; i < models.items.size() && i < m_views.size(); ++i) {
      auto &model = models.items[i];
      const auto &entry = m_views[i];
      bool const shouldSkipRender = !model.dirty && !model.propsDirty;

      if (!shouldSkipRender) entry.renderFn(model.root);
    }
  }

  void handleViewPoped(const BaseView *view) {
    auto it =
        std::ranges::find_if(m_views, [view](const ViewEntry &entry) { return entry.baseView == view; });

    if (it == m_views.end()) return;
    if (m_views.size() > 1) { emitviewPoped(); }

    m_views.pop_back();
  }

private:
  ExtensionActionPanelBuilder::NotifyFn makeNotifyFn() {
    return [this](const QString &handler, const QJsonArray &args) {
      m_eventCore->emithandlerActivated(handler.toStdString(), qJsonValueToGlazeGeneric(args).get_array());
    };
  }

  static PopToRootType mapPopToRoot(tsapi::PopToRootType type) {
    switch (type) {
    case tsapi::PopToRootType::Immediate:
      return PopToRootType::Immediate;
    case tsapi::PopToRootType::Suspended:
      return PopToRootType::Suspended;
    default:
      return PopToRootType::Default;
    }
  }

  static ToastStyle mapToastStyle(tsapi::ToastStyle style) {
    switch (style) {
    case tsapi::ToastStyle::Success:
      return ToastStyle::Success;
    case tsapi::ToastStyle::Info:
      return ToastStyle::Info;
    case tsapi::ToastStyle::Warning:
      return ToastStyle::Warning;
    case tsapi::ToastStyle::Error:
      return ToastStyle::Danger;
    case tsapi::ToastStyle::Dynamic:
      return ToastStyle::Dynamic;
    default:
      return ToastStyle::Success;
    }
  }

  std::vector<ViewEntry> m_views;
  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  NavigationController *m_navigation;
  std::shared_ptr<ExtensionCommand> m_command;
  tsapi::AbstractEventCore *m_eventCore;
  ToastService &m_toast;
};
