#pragma once
#include "extension/extension-command-controller.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-view-wrapper.hpp"
#include "navigation-controller.hpp"
#include "theme.hpp"
#include "ui/image/url.hpp"
#include <qobject.h>

class ExtensionNavigationController : public QObject {
  std::vector<ExtensionViewWrapper *> m_views;
  std::shared_ptr<ExtensionCommand> m_command;
  NavigationController *m_navigation;
  std::unique_ptr<ExtensionCommandController> m_controller;
  QString m_sessionId;
  bool m_devMode = false;

  QString defaultNavigationTitle() { return m_command->name(); }

public:
  ExtensionCommandController *controller() const { return m_controller.get(); }

  NavigationController *handle() const { return m_navigation; }

  void pushView() {
    auto view = new ExtensionViewWrapper(m_controller.get());

    m_navigation->pushView(view);
    m_navigation->setNavigationTitle(defaultNavigationTitle());
    m_navigation->setNavigationIcon(m_command->iconUrl());
    m_views.emplace_back(view);
  }

  std::vector<ExtensionViewWrapper *> views() const { return m_views; }

  void handleViewPoped(const BaseView *view) {
    // Only handle pops for ExtensionViewWrapper views that are in our stack
    auto wrapper = dynamic_cast<const ExtensionViewWrapper *>(view);
    if (!wrapper) return;

    auto it = std::find(m_views.begin(), m_views.end(), const_cast<ExtensionViewWrapper *>(wrapper));
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
