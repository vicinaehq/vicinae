#pragma once
#include "common.hpp"
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/shortcut-button.hpp"
#include "ui/toast.hpp"
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

public:
  GlobalBar(ApplicationContext &ctx);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  ApplicationContext &m_ctx;
  QStackedWidget *m_leftWidget = new QStackedWidget;
  NavigationStatusWidget *m_status = new NavigationStatusWidget;
  ShortcutButton *m_primaryActionButton = new ShortcutButton;
  ShortcutButton *m_actionButton = new ShortcutButton;
  ToastWidget *m_toast = new ToastWidget;

  void handleToast(const Toast *toast);
  void handleToastDestroyed(const Toast *toast);
  void handleViewStateChange(const NavigationController::ViewState &state);
  void actionsChanged(const ActionPanelState &actions);
  void handleActionPanelVisiblityChange(bool visible);

  void setupUI();
};
