#pragma once
#include "argument.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include <QObject>

class ApplicationContext;
class ActionPanelState;
class Toast;
class ImageURL;
class QKeyEvent;

class BaseView : public QObject {

public:
  void createInitialize();
  bool isInitialized();
  void popSelf();

  void setProxy(BaseView *proxy);

  void setActions(std::unique_ptr<ActionPanelState> actions);
  void clearActions();

  virtual bool supportsSearch() const;
  virtual bool searchInteractive() const;
  void executePrimaryAction();
  virtual bool needsGlobalStatusBar() const;
  virtual bool needsGlobalTopBar() const;
  bool isLoading() const;

  virtual QString initialSearchPlaceholderText() const { return QString(); }
  virtual QString initialNavigationTitle() const { return command()->info().name(); }
  virtual ImageURL initialNavigationIcon() const { return command()->info().iconUrl(); }

  virtual void initialize();
  void activate();
  void deactivate();
  virtual void argumentValuesChanged(const std::vector<std::pair<QString, QString>> &arguments) {}

  virtual void textChanged(const QString &text);

  QString navigationTitle() const;

  virtual void onActivate();
  virtual void onDeactivate();
  virtual bool onBackspace() { return false; }
  virtual bool showBackButton() const { return true; }
  virtual void beforePop() {}

  void activateCompleter(const ArgumentList &args, const ImageURL &icon);

  void setContext(ApplicationContext *ctx);
  void setCommandController(CommandController *commandController) { m_cmd = commandController; }

  CommandController *command() const { return m_cmd; }

  ApplicationContext *context() const;

  void destroyCompleter();

  QString searchPlaceholderText() const;
  void setSearchPlaceholderText(const QString &value) const;

  void setTopBarVisiblity(bool visible);
  void setSearchVisibility(bool visible);
  void setSearchInteractive(bool interactive);
  void setStatusBarVisiblity(bool visible);

  void clearSearchText();
  QString searchText() const;
  void setSearchText(const QString &value);

  virtual bool inputFilter(QKeyEvent *event);
  virtual void setNavigationIcon(const ImageURL &icon);

  void setNavigation(const QString &title, const ImageURL &icon);
  void setNavigationTitle(const QString &title);
  void setLoading(bool value);

  virtual std::vector<QString> argumentValues() const;

  BaseView(QObject *parent = nullptr);

private:
  bool m_initialized = false;
  ApplicationContext *m_ctx = nullptr;
  CommandController *m_cmd = nullptr;

  const BaseView *m_navProxy = this;
};
