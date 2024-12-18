#pragma once
#include "app-database.hpp"
#include "calculator-database.hpp"
#include "clipboard-service.hpp"
#include "extension_manager.hpp"
#include "quicklist-database.hpp"
#include <jsoncpp/json/value.h>
#include <qboxlayout.h>
#include <stack>

#include "ui/action_popover.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

template <class T> using Service = T &;

class View;
class ViewCommand;
class ExtensionView;
class Command;

class AppWindow : public QMainWindow {
  Q_OBJECT

public:
  std::stack<CommandObject *> commandStack;
  std::stack<QString> queryStack;

  std::stack<View *> navigationStack;
  ViewCommand *currentCommand = nullptr;

  std::unique_ptr<QuicklistDatabase> quicklinkDatabase;
  std::unique_ptr<CalculatorDatabase> calculatorDatabase;
  std::unique_ptr<ClipboardService> clipboardService;
  std::unique_ptr<AppDatabase> appDb;
  std::unique_ptr<ExtensionManager> extensionManager;

  void pushView(View *view);
  void pushExtensionView(ExtensionView *view);
  void popCurrentView();
  void popToRootView();
  void disconnectView(View &view);
  void connectView(View &view);

  template <typename T> Service<T> service() const;

  TopBar *topBar = nullptr;
  StatusBar *statusBar = nullptr;
  ActionPopover *actionPopover = nullptr;
  QVBoxLayout *layout = nullptr;
  QWidget *defaultWidget = new QWidget();

  void pushCommandObject(std::shared_ptr<ICommandFactory> factory);
  void popCommandObject();
  void popToRoot();

  void launchCommand(ViewCommand *cmd);

  AppWindow(QWidget *parent = 0);

public slots:
  bool eventFilter(QObject *obj, QEvent *event) override;
};
