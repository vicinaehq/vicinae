#pragma once
#include "navigation-controller.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/top_bar.hpp"
#include <qcoreevent.h>
#include <qobject.h>
#include <qwidget.h>

class GlobalHeader : public QWidget {
public:
  GlobalHeader(NavigationController &controller);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NavigationController &m_navigation;
  SearchBar *m_input = new SearchBar(this);
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);

  void setupUI();
  void handleViewStateChange(const NavigationController::ViewState &state);
  void handleSearchPop();
};
