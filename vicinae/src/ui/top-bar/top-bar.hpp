#pragma once
#include "navigation-controller.hpp"
#include <qwidget.h>

class NavigationController;
class SearchBar;
class IconButton;
class QStackedWidget;
class HorizontalLoadingBar;
class ArgCompleter;

class GlobalHeader : public QWidget {
public:
  GlobalHeader(NavigationController &controller);
  SearchBar *input() const;
  void setLoadingBarVisibility(bool value);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void showEvent(QShowEvent *event) override;

private:
  NavigationController &m_navigation;
  SearchBar *m_input;
  HorizontalLoadingBar *m_loadingBar;
  IconButton *m_backButton;
  QWidget *m_backButtonSpacer = new QWidget;
  QStackedWidget *m_accessoryContainer;
  ArgCompleter *m_completer;
  QWidget *m_left = nullptr;

  void setAccessory(QWidget *accessory);
  void clearAccessory();

  void setupUI();
  void handleViewStateChange(const NavigationController::ViewState &state);

  /**
   * Controls whether the loading bar is visible or not, not if it's displaying its loading indicator.
   * When nothing is loading the bar is still rendered as a regular separator: this allows disabling that.
   */

  void handleSearchPop();
  bool filterInputEvents(QEvent *event);
  void handleTextEdited(const QString &text);
};
