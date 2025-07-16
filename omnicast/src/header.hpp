#pragma once
#include "navigation-controller.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/icon-button.hpp"
#include "ui/top_bar.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qobject.h>
#include <qwidget.h>

class GlobalHeader : public QWidget {
public:
  GlobalHeader(NavigationController &controller);

  SearchBar *input() const;

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  NavigationController &m_navigation;
  SearchBar *m_input = new SearchBar(this);
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  IconButton *m_backButton = new IconButton;
  QWidget *m_backButtonSpacer = new QWidget;

  void setupUI();
  void handleViewStateChange(const NavigationController::ViewState &state);
  void handleSearchPop();
  bool filterInputEvents(QEvent *event);
  void handleTextEdited(const QString &text);
};
