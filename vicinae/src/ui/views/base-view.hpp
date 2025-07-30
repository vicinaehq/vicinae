#pragma once
#include "argument.hpp"
#include <qwidget.h>

class ApplicationContext;
class ActionPanelState;
class Toast;
class OmniIconUrl;

class BaseView : public QWidget {
  bool m_initialized = false;
  ApplicationContext *m_ctx = nullptr;
  const BaseView *m_navProxy = this;

public:
  void createInitialize();
  bool isInitialized();
  void popSelf();

  /**
   * Forward navigation opercontext()ations to `other` instead of this view.
   * Allows to nest view with only one effectively having navigation responsability.
   * In most cases, you should not use this. This is mainly used for extensions.
   */
  void setProxy(BaseView *proxy);

  void setActions(std::unique_ptr<ActionPanelState> actions);

  /**
   * Whether to show the search bar for this view. Calling setSearchText or searchText() is still
   * valid but will always return the empty string.
   */
  virtual bool supportsSearch() const;
  void executePrimaryAction();
  virtual bool needsGlobalStatusBar() const;
  virtual bool needsGlobalTopBar() const;

  /**
   * Called before the view is first shown on screen.
   * You can use this hook to setup UI.
   */
  virtual void initialize();
  void activate();
  void deactivate();
  virtual void argumentValuesChanged(const std::vector<std::pair<QString, QString>> &arguments) {}

  /**
   * Received when the global text search bar updates.
   */
  virtual void textChanged(const QString &text);

  QString navigationTitle() const;
  void setSearchAccessory(QWidget *accessory);

  /**
   * Called when the view becomes visible. This is called the first time the view is shown
   * (right after `initialize`) but also after a view that was pushed on top of it was poped.
   */
  virtual void onActivate();

  /**
   * Called when the view becomes hidden. This is called before the view is poped or when
   * another view is pushed on top of it.
   */
  virtual void onDeactivate();

  void activateCompleter(const ArgumentList &args, const OmniIconUrl &icon) {
    // m_uiController->activateCompleter(args, icon);
  }

  void setContext(ApplicationContext *ctx);

  /**
   * The entire application context.
   * You normally do not need to use this directly. Use the helper methods instead.
   * Note that the returned context is only valid if the view is tracked by the navigation
   * controller. A view not (yet) tracked will have this function return a null pointer.
   */
  ApplicationContext *context() const;

  void destroyCompleter();

  virtual QWidget *searchBarAccessory() const { return nullptr; }

  QString searchPlaceholderText() const;

  void setSearchPlaceholderText(const QString &value) const;

  void clearSearchAccessory();

  void setTopBarVisiblity(bool visible);

  void setSearchVisibility(bool visible);

  void setStatusBarVisiblity(bool visible);

  void clearSearchText();

  /**
   * The current search text for this view. If not applicable, do not implement.
   */
  QString searchText() const;

  /**
   * Set the search text for the current view, if applicable
   */
  void setSearchText(const QString &value);

  /**
   * Allows the view to filter input from the main search bar before the input itself
   * processes it.
   * For instance, this allows a list view to capture up and down events to move the position in the list.
   * Or, as an example that actually modifies the input behaviour, a grid list (with horizontal controls)
   * can repurpose the left and right keys to navigate the list, while they would normally move the text
   * cursor.
   *
   * In typical QT event filter fashion, this function should return false if the key is left for the input
   * to handle, or true if it needs to be ignored.
   *
   */
  virtual bool inputFilter(QKeyEvent *event);

  /**
   * Set the navigation icon, if applicable
   */
  virtual void setNavigationIcon(const OmniIconUrl &icon);

  void setNavigation(const QString &title, const OmniIconUrl &icon);

  void setNavigationTitle(const QString &title);

  void setLoading(bool value);

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const;

  BaseView(QWidget *parent = nullptr);
};
