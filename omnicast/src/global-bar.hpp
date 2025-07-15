#pragma once
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/typography/typography.hpp"
#include <qstackedwidget.h>
#include <qwidget.h>

class NavigationStatusWidget : public QWidget {
public:
  NavigationStatusWidget();

  void setTitle(const QString &title);
  void setIcon(const OmniIconUrl &icon);

private:
  void setupUI();

  TypographyWidget *m_navigationTitle = new TypographyWidget(this);
  Omnimg::ImageWidget *m_navigationIcon = new Omnimg::ImageWidget(this);
};

class GlobalBar : public QWidget {
  // can show navigation icon + title or toast
  QStackedWidget *m_leftWidget = new QStackedWidget;
  NavigationStatusWidget *m_status = new NavigationStatusWidget;

public:
  GlobalBar(NavigationController &controller);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  NavigationController &m_navigation;

  void setupUI();
};
