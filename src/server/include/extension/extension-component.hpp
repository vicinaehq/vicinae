#pragma once
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ActionPannelWidget;

class AbstractExtensionRootComponent : public QWidget {
  Q_OBJECT
  AppWindow &app;

  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    emit componentShown();
    qCritical() << "show event component";
  }

public:
  AbstractExtensionRootComponent(AppWindow &app) : app(app) {}

  virtual bool inputFilter(QKeyEvent *event) { return false; }

  virtual void onMount() {}

  QString searchText() const { return ""; }
  void setSearchText(const QString &text) {}

  virtual void onSearchChanged(const QString &text) {}

  ActionPannelWidget *actionPannel() const { return nullptr; }

  virtual void render(const RenderModel &model) = 0;

signals:
  void notifyEvent(const QString &handler, const QJsonArray &args) const;
  void updateActionPannel(const ActionPannelModel &model) const;
  void componentShown() const;
  void setNavigationTitle(const QString &text);
  void setSearchPlaceholderText(const QString &text);
  void setSearchAccessory(QWidget *accessory);
  void selectPrimaryAction();
  void setLoading(bool loading);
};
