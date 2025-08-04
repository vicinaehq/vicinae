#pragma once
#include <qwidget.h>
#include "navigation-controller.hpp"
#include "ui/image/omnimg.hpp"

class ApplicationContext;
class TypographyWidget;
class ImageURL;
class ToastWidget;
class ShortcutButton;
class QStackedWidget;
class ActionPanelState;
class Toast;

class NavigationStatusWidget : public QWidget {
public:
  NavigationStatusWidget();

  void setTitle(const QString &title);
  void setIcon(const ImageURL &icon);

private:
  void setupUI();

  TypographyWidget *m_navigationTitle;
  Omnimg::ImageWidget *m_navigationIcon = new Omnimg::ImageWidget(this);
};

class GlobalBar : public QWidget {

public:
  GlobalBar(ApplicationContext &ctx);

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  ApplicationContext &m_ctx;
  QStackedWidget *m_leftWidget;
  NavigationStatusWidget *m_status;
  ShortcutButton *m_primaryActionButton;
  ShortcutButton *m_actionButton;
  ToastWidget *m_toast;

  void handleToast(const Toast *toast);
  void handleToastDestroyed(const Toast *toast);
  void handleViewStateChange(const NavigationController::ViewState &state);
  void actionsChanged(const ActionPanelState &actions);
  void handleActionPanelVisiblityChange(bool visible);

  void setupUI();
};
