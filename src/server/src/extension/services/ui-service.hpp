#pragma once
#include "extension/extension-navigation-controller.hpp"
#include "extension/services/tsapi-image.hpp"
#include "generated/tsapi.hpp"
#include "navigation-controller.hpp"
#include "qml/view-utils.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert/alert.hpp"
#include <QClipboard>
#include <QGuiApplication>
#include <QtConcurrent/QtConcurrent>
#include <qfuturewatcher.h>

class ExtUIService : public QObject, public tsapi::AbstractUI {
  Q_OBJECT

  using Void = tsapi::Result<void>;

public:
  ExtUIService(tsapi::RpcTransport &transport, ExtensionNavigationController *navigation, ToastService &toast,
               QObject *parent = nullptr)
      : QObject(parent), AbstractUI(transport), m_navigation(navigation), m_toast(toast) {
    connect(&m_modelWatcher, &QFutureWatcher<ParsedRenderData>::finished, this, &ExtUIService::modelCreated);
  }

  Void::Future render(const std::string &json) override {
    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);

    if (parseError.error) { return Void::fail("Failed to parse render tree"); }

    auto views = doc.object().value("views").toArray();

    if (m_modelWatcher.isRunning()) {
      m_modelWatcher.cancel();
      m_modelWatcher.waitForFinished();
    }

    m_modelWatcher.setFuture(QtConcurrent::run([views]() { return ModelParser().parse(views); }));

    return Void::ok();
  }

  Void::Future showToast(const std::string &id, const std::string &title, const std::string &message,
                         const tsapi::ToastStyle &style) override {
    m_toast.setToast(QString::fromStdString(title), mapToastStyle(style), QString::fromStdString(message));
    return Void::ok();
  }

  Void::Future updateToast(const std::string &id, const std::string &title) override { return Void::ok(); }

  Void::Future hideToast(const std::string &id) override {
    m_toast.clear();
    return Void::ok();
  }

  Void::Future showHud(const std::string &text, const bool &clear_root,
                       const tsapi::PopToRootType &popToRoot) override {
    m_navigation->handle()->closeWindow({
        .popToRootType = mapPopToRoot(popToRoot),
        .clearRootSearch = clear_root,
    });
    m_navigation->handle()->showHud(QString::fromStdString(text));
    return Void::ok();
  }

  Void::Future closeMainWindow(const bool &clearRoot, const tsapi::PopToRootType &popToRoot) override {
    using namespace std::chrono_literals;
    m_navigation->handle()->closeWindow(
        {.popToRootType = mapPopToRoot(popToRoot), .clearRootSearch = clearRoot}, 50ms);
    return Void::ok();
  }

  Void::Future popToRoot(const bool &clearSearchBar) override {
    m_navigation->handle()->popToRoot({.clearSearch = clearSearchBar});
    return Void::ok();
  }

  tsapi::Result<bool>::Future confirmAlert(const tsapi::ConfirmAlertPayload &payload) override {
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

    m_navigation->handle()->setDialog(alert);

    return future;
  }

  Void::Future pushView() override {
    m_navigation->pushView();
    return Void::ok();
  }

  Void::Future popView() override {
    m_navigation->popView();
    return Void::ok();
  }

  Void::Future setSearchText(const std::string &text) override {
    m_navigation->handle()->setSearchText(QString::fromStdString(text));
    return Void::ok();
  }

  tsapi::Result<std::string>::Future getSelectedText() override {
    auto text = QGuiApplication::clipboard()->text(QClipboard::Mode::Selection);
    return tsapi::Result<std::string>::ok(text.toStdString());
  }

private slots:
  void modelCreated() {
    if (m_modelWatcher.isCanceled()) return;

    const auto &views = m_navigation->views();
    auto models = m_modelWatcher.result();

    for (size_t i = 0; i < models.items.size() && i < views.size(); ++i) {
      auto &model = models.items[i];
      const auto &entry = views[i];
      bool const shouldSkipRender = !model.dirty && !model.propsDirty;

      if (!shouldSkipRender) entry.renderFn(model.root);
    }
  }

private:
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

  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  ExtensionNavigationController *m_navigation;
  ToastService &m_toast;
};
