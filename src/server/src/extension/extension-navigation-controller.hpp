#pragma once
#include "extension/extension-command-controller.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-view-wrapper.hpp"
#include "navigation-controller.hpp"
#include "qml/qml-extension-view-host.hpp"
#include "theme.hpp"
#include "ui/image/url.hpp"
#include <qobject.h>

class ExtensionNavigationController : public QObject {
public:
  struct ViewEntry {
    BaseView *baseView;
    std::function<void(const RenderModel &)> renderFn;
  };

private:
  std::vector<ViewEntry> m_views;
  std::shared_ptr<ExtensionCommand> m_command;
  NavigationController *m_navigation;
  std::unique_ptr<ExtensionCommandController> m_controller;
  QString m_sessionId;
  bool m_devMode = false;

  QString defaultNavigationTitle() { return m_command->name(); }

  void handleFallback(QmlExtensionViewHost *host, const RenderModel &model) {
    // The QML host can't handle this view type (form/detail).
    // Replace with a classic ExtensionViewWrapper.
    auto wrapper = new ExtensionViewWrapper(m_controller.get());

    // Find the ViewEntry for this host and replace it
    for (auto &entry : m_views) {
      if (entry.baseView == host) {
        entry.baseView = wrapper;
        entry.renderFn = [wrapper](const RenderModel &m) { wrapper->render(m); };
        break;
      }
    }

    // replaceView() defers via QTimer::singleShot(0) to set up context.
    // Defer render to the next iteration so wrapper has context when render() runs.
    m_navigation->replaceView(wrapper);
    QTimer::singleShot(0, wrapper, [wrapper, model]() { wrapper->render(model); });
  }

public:
  ExtensionCommandController *controller() const { return m_controller.get(); }

  NavigationController *handle() const { return m_navigation; }

  void pushView() {
    auto host = new QmlExtensionViewHost(m_controller.get());

    connect(host, &QmlExtensionViewHost::fallbackRequired, this,
            [this, host](const RenderModel &model) { handleFallback(host, model); });

    m_navigation->pushView(host);
    m_navigation->setNavigationTitle(defaultNavigationTitle());
    m_navigation->setNavigationIcon(m_command->iconUrl());

    m_views.push_back({host, [host](const RenderModel &m) { host->render(m); }});
  }

  const std::vector<ViewEntry> &views() const { return m_views; }

  void handleViewPoped(const BaseView *view) {
    // Check if the popped view is in our stack
    auto it = std::find_if(m_views.begin(), m_views.end(),
                           [view](const ViewEntry &entry) { return entry.baseView == view; });
    if (it == m_views.end()) return;

    if (m_views.size() > 1) { m_controller->notify("pop-view", {}); }

    m_views.pop_back();
  }

  void popView() { m_navigation->popCurrentView(); }

  void setSessionId(const QString &id) {
    m_sessionId = id;
    m_controller->setSessionId(id);
  }

  void setDevMode(bool mode) { m_devMode = mode; }

  void setSubtitleOverride(const std::optional<QString> &subtitle) {
    m_command->setSubtitleOverride(subtitle);
  }

  ExtensionNavigationController(const std::shared_ptr<ExtensionCommand> &command,
                                NavigationController *navigation, ExtensionManager *manager)
      : m_command(command), m_navigation(navigation),
        m_controller(std::make_unique<ExtensionCommandController>(manager)) {
    connect(navigation, &NavigationController::viewPoped, this,
            &ExtensionNavigationController::handleViewPoped);
  }
};
