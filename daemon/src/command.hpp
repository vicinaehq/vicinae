#pragma once
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

public slots:
  virtual void onSearchChanged(const QString &s) {}

signals:
  void launchCommand();
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
  void extensionMessage(const Message &msg) {}

  void render(QJsonObject data) {
    auto tree = data["root"].toObject();
    auto rootType = tree["type"].toString();

    if (rootType == "list") {
    }
  }

public:
  ExtensionView(AppWindow &app) : View(app) { widget = new QWidget(); }
};

class Command {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
};

/**
 */
class HeadlessCommand : public Command {
  virtual void load() = 0;
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
