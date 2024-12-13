#pragma once
#include "app.hpp"
#include "extension_manager.hpp"
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <variant>

class AppWindow;

class View : public QObject {
  Q_OBJECT
  AppWindow &app;

protected:
public:
  QWidget *widget;
  View(AppWindow &app) : app(app) {}

  template <typename T> Service<T> service() { return app.service<T>(); }

public slots:
  virtual void onSearchChanged(const QString &s) {}

signals:
  void launchCommand(ViewCommand *command);
  void pushView(View *view);
  void pop();
  void popToRoot();
};

class ExtensionList {};

class ExtensionView : public View {
  Q_OBJECT

  using RootComponent = std::variant<ExtensionList>;

  std::optional<RootComponent> component;

public slots:
  void extensionMessage(const Message &msg) {
    qDebug() << "got extension message" << msg.type << "from view";
  }

  void render(QJsonObject data) {
    auto tree = data["root"].toObject();
    auto rootType = tree["type"].toString();

    if (rootType == "list") {
    }
  }

public:
  ExtensionView(AppWindow &app, const QString &name) : View(app) {
    widget = new QLabel(name);
  }

  ~ExtensionView() { qDebug() << "Destroy extension view"; }
};

class Command {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

/**
 */
class HeadlessCommand : public Command {
  virtual void load() = 0;
};

class ExtensionCommand : public ViewCommand {
  QString cmd;
  QString ext;

public:
  ExtensionCommand(const QString &extensionId, const QString &cmd)
      : ext(extensionId), cmd(cmd) {}

  View *load(AppWindow &app) override {
    app.extensionManager->activateCommand(ext, cmd);

    return new ExtensionView(app, cmd);
  }

  void unload(AppWindow &app) override {
    app.extensionManager->deactivateCommand(ext, cmd);
  }
};

using CommandType = std::variant<ViewCommand>;

/*
class CalculatorHistoryList : public View {
public:
  CalculatorHistoryList() { widget = new QLabel(); }

  void onSearchChanged(const QString &s) override {
    qDebug() << "on search changed";

    emit launchCommand();

    emit pushView(std::make_unique<CalculatorHistoryList>());
  }
};

class CalculatorHistoryCommand : public ViewCommand {
  UniqueView load() { return std::make_unique<CalculatorHistoryList>(); }
};
*/
