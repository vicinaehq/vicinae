#pragma once
#include "navigation-controller.hpp"
#include <qwidget.h>

class NavigationController;
class SearchBar;
class IconButton;
class QStackedWidget;
class HorizontalLoadingBar;

class GlobalHeader : public QWidget {
public:
  GlobalHeader(NavigationController &controller);
  SearchBar *input() const;

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NavigationController &m_navigation;
  SearchBar *m_input;
  HorizontalLoadingBar *m_loadingBar;
  IconButton *m_backButton;
  QWidget *m_backButtonSpacer = new QWidget;
  QStackedWidget *m_accessoryContainer;

  void setAccessory(QWidget *accessory);
  void clearAccessory();

  void setupUI();
  void handleViewStateChange(const NavigationController::ViewState &state);
  void handleSearchPop();
  bool filterInputEvents(QEvent *event);
  void handleTextEdited(const QString &text);
};
