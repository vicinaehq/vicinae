#pragma once
#include "app.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qwidget.h>

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

  QString searchText() const { return app.topBar->input->text(); }
  void setSearchText(const QString &text) {
    bool changed = app.topBar->input->text() != text;

    if (changed) {
      qDebug() << "changed!!!!" << app.topBar->input->text() << "VS new" << text;
      app.topBar->input->setText(text);
      // emit app.topBar->input->textEdited(text);
    }
  }
  void setSearchPlaceholderText(const QString &text) { app.topBar->input->setPlaceholderText(text); }
  void setNavigationTitle(const QString &text) {
    if (!isVisible()) return;
    app.statusBar->setNavigationTitle(text);
  }
  void setSearchAccessory(QWidget *accessory) { app.topBar->setAccessoryWidget(accessory); }
  void selectPrimaryAction() { app.selectPrimaryAction(); }

  virtual void onSearchChanged(const QString &text) {}

  void setLoading(bool loading) {
    if (!isVisible()) return;
    app._loadingBar->setStarted(loading);
  }

  ActionPannelWidget *actionPannel() const { return app.actionPannel; }

  virtual void render(const RenderModel &model) = 0;

signals:
  void notifyEvent(const QString &handler, const std::vector<QJsonValue> &args) const;
  void updateActionPannel(const ActionPannelModel &model) const;
  void componentShown() const;
};
