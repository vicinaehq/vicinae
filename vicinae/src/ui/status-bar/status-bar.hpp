#pragma once
#include <qwidget.h>
#include "navigation-controller.hpp"
#include "ui/image/image.hpp"

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
  void setSuffixIcon(const std::optional<ImageURL> &icon);

private:
  void setupUI();

  TypographyWidget *m_navigationTitle;
  ImageWidget *m_navigationIcon = new ImageWidget(this);
  ImageWidget *m_suffixIcon = new ImageWidget(this);
};

class GlobalBar : public QWidget {

public:
  GlobalBar(ApplicationContext &ctx);

private:
  ApplicationContext &m_ctx;
  QStackedWidget *m_leftWidget;
  NavigationStatusWidget *m_status;
  ShortcutButton *m_primaryActionButton;
  ShortcutButton *m_actionButton;
  ToastWidget *m_toast;

  void handleToast(const Toast *toast);
  void handleToastDestroyed();
  void handleViewStateChange(const NavigationController::ViewState &state);
  void actionsChanged(const ActionPanelState &actions);
  void handleActionPanelVisiblityChange(bool visible);

  void setupUI();
};
