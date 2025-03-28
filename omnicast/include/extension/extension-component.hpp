#pragma once
#include "app.hpp"
#include <qtmetamacros.h>

class AbstractExtensionRootComponent : public QWidget {
  Q_OBJECT
  AppWindow &app;

public:
  AbstractExtensionRootComponent(AppWindow &app) : app(app) {}

  void setSearchText(const QString &text) { app.topBar->input->setText(text); }
  void setSearchPlaceholderText(const QString &text) { app.topBar->input->setPlaceholderText(text); }
  void setNavigationTitle(const QString &text) { app.statusBar->setNavigationTitle(text); }

  virtual void onSearchChanged(const QString &text) {}

  ActionPannelWidget *actionPannel() const { return app.actionPannel; }

  virtual void render(const RenderModel &model) = 0;

signals:
  void notifyEvent(const QString &handler, const QJsonObject &payload) const;
};
