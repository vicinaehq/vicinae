#pragma once
#include "argument.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include <qjsonobject.h>
#include <qwidget.h>

class ApplicationContext;
class ActionPanelState;
class Toast;
class ImageURL;

class BaseView : public QWidget {

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
  void clearActions();

  /**
   * Whether to show the search bar for this view. Calling setSearchText or searchText() is still
   * valid but will always return the empty string.
   */
  virtual bool supportsSearch() const;
  void executePrimaryAction();
  virtual bool needsGlobalStatusBar() const;
  virtual bool needsGlobalTopBar() const;
  bool isLoading() const;

  /**
   * Search placeholder text to set when first rendering the view, if
   * applicable.
   */
  virtual QString initialSearchPlaceholderText() const { return QString(); }
  virtual QString initialNavigationTitle() const { return command()->info().name(); }
  virtual ImageURL initialNavigationIcon() const { return command()->info().iconUrl(); }

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
  void setSearchAccessoryVisiblity(bool value);
  QWidget *currentSearchAccessory() const;

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

  /**
   * When backspace is pressed and the search bar is empty.
   * The return value indicates whether we should consume the backspace
   * handling and not let it propagate. By returning true, you can effectively
   * override the normal handling of backspace completely.
   */
  virtual bool onBackspace() { return false; }

  /**
   * Whether this view should show a back button if it's not the only one on the stack.
   * This is tweakable so that some pushed views can look as if they are the only view on the stack,
   * even if they are technically not.
   */
  virtual bool showBackButton() const { return true; }

  virtual void beforePop() {}

  void activateCompleter(const ArgumentList &args, const ImageURL &icon);

  void setContext(ApplicationContext *ctx);
  void setCommandController(CommandController *commandController) { m_cmd = commandController; }

  CommandController *command() const { return m_cmd; }

  /**
   * Sets the command this view belongs to. You normally shouldn't call this,
   * as this is automatically injected by the
   */

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
  virtual void setNavigationIcon(const ImageURL &icon);

  void setNavigation(const QString &title, const ImageURL &icon);

  void setNavigationTitle(const QString &title);

  void setLoading(bool value);

  /**
   * The dynamic arguments for this view. Used by some actions.
   */
  virtual std::vector<QString> argumentValues() const;

  BaseView(QWidget *parent = nullptr);

private:
  bool m_initialized = false;
  ApplicationContext *m_ctx = nullptr;
  CommandController *m_cmd = nullptr;

  const BaseView *m_navProxy = this;
};
