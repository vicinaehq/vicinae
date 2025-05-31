#pragma once
#include "common.hpp"
#include "ui/toast.hpp"
#include <qobject.h>

class BaseView;

// general top level UI operation mediator
// Anything in omnicast can ask to push a new view and the signal will be handled by the main AppWindow
class UIController : public QObject {
  Q_OBJECT
  BaseView *m_view = nullptr;

public:
  UIController() {}
  ~UIController() {}

  void closeWindow() { emit closeWindowRequested(); }
  void popToRoot() { emit popToRootRequested(); }
  void showHUD(const QString &title) { emit showHUDRequested(title); }
  void pushView(BaseView *view, const PushViewOptions &opts = {}) const {
    emit pushViewRequested(view, opts);
  }
  void replaceView(BaseView *previous, BaseView *next) { emit replaceViewRequested(previous, next); }
  void replaceCurrentView(BaseView *next) { emit replaceViewRequested(topView(), next); }
  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd) const { emit launchCommandRequested(cmd); }
  void launchCommand(const QString &cmdId) const { /*emit launchCommandRequested(cmdId);*/ }
  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) const {
    emit showToastRequested(title, priority);
  }
  void popView() const { emit popViewRequested(); }
  void setTopView(BaseView *view) { m_view = view; }
  BaseView *topView() const { return m_view; }

signals:
  void pushViewRequested(BaseView *view, const PushViewOptions &opts) const;
  void popViewRequested() const;
  void popToRootRequested() const;
  void closeWindowRequested() const;
  void launchCommandRequested(const std::shared_ptr<AbstractCmd> &cmd) const;
  void showHUDRequested(const QString &title) const;
  void showToastRequested(const QString &title, ToastPriority priority) const;
  void replaceViewRequested(BaseView *previous, BaseView *next) const;
};
