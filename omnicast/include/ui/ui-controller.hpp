#pragma once
#include "common.hpp"
#include "ui/toast.hpp"
#include <qobject.h>

class BaseView;

class UIController : public QObject {
  Q_OBJECT
  BaseView *m_view = nullptr;

public:
  UIController() {}
  ~UIController() {}

  void closeWindow() { emit closeWindowRequested(); }
  void popToRoot() { emit popToRootRequested(); }
  void showHUD(const QString &title) { emit showHUDRequested(title); }
  void pushView(BaseView *view) const { emit pushViewRequested(view); }
  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd) const { emit launchCommandRequested(cmd); }
  void launchCommand(const QString &cmdId) const { /*emit launchCommandRequested(cmdId);*/ }
  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) const {
    emit showToastRequested(title, priority);
  }
  void popView() const { emit popViewRequested(); }
  void setTopView(BaseView *view) { m_view = view; }
  BaseView *topView() const { return m_view; }

signals:
  void pushViewRequested(BaseView *view) const;
  void popViewRequested() const;
  void popToRootRequested() const;
  void closeWindowRequested() const;
  void launchCommandRequested(const std::shared_ptr<AbstractCmd> &cmd) const;
  // void launchCommandRequested(const QString &commandId) const;
  void showHUDRequested(const QString &title) const;
  void showToastRequested(const QString &title, ToastPriority priority) const;
};
