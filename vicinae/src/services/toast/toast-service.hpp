#pragma once
#include "common.hpp"
#include "ui/toast/toast.hpp"
#include <deque>
#include <qobject.h>
#include <qstring.h>
#include <qtmetamacros.h>

class Toast : public QObject {
  Q_OBJECT

signals:
  void updated() const;
  void destroyRequested() const;

public:
  Toast(const QString &title, ToastStyle priority) : m_title(title), m_priority(priority) {}

  const QString &title() const { return m_title; }

  ToastStyle priority() const { return m_priority; }

  void setTitle(const QString &title) {
    m_title = title;
    emit updated();
  }

  void setPriority(ToastStyle priority) { m_priority = priority; }

  void update() const { emit updated(); }

  void close() const { emit destroyRequested(); }

  ~Toast() { close(); }

  QString m_title;
  ToastStyle m_priority;
};

class ToastService : public QObject {
  Q_OBJECT

signals:
  void toastActivated(const Toast *toast) const;
  void toastHidden() const;

public:
  Toast *currentToast() { return m_queue.empty() ? nullptr : m_queue.back().get(); }

  void success(const QString &title) { setToast(title, ToastStyle::Success); }
  void failure(const QString &title) { setToast(title, ToastStyle::Danger); }
  void dynamic(const QString &title) { setToast(title, ToastStyle::Dynamic); }

  void clear() {
    m_queue.clear();
    emit toastHidden();
  }

  void setToast(const QString &title, ToastStyle priority = ToastStyle::Success, int duration = 2000) {
    // for now we only handle one toast and we replace it every time. We will use the stack if we find the
    // need to handle many toasts concurrently.
    clear();
    auto toast = std::shared_ptr<Toast>(new Toast(title, priority), QObjectDeleter());

    if (priority != ToastStyle::Dynamic) {
      QTimer::singleShot(duration, toast.get(), [this, toast]() { toast->close(); });
    }
    registerToast(toast);
  }

private:
  void updateCurrent() {
    if (m_queue.empty()) {
      emit toastHidden();
      return;
    }
    emit toastActivated(m_queue.back().get());
  }

  void destroyToast(Toast *toast) {
    auto pred = [&](auto &&ptr) { return ptr.get() == toast; };
    if (auto it = std::ranges::find_if(m_queue, pred); it != m_queue.end()) {
      m_queue.erase(it);
      updateCurrent();
    }
  }

  void registerToast(std::shared_ptr<Toast> toast) {
    m_queue.push_back(toast);
    updateCurrent();
    connect(toast.get(), &Toast::destroyRequested, this,
            [this, toast = toast.get()]() { destroyToast(toast); });
    connect(toast.get(), &Toast::updated, this, &ToastService::updateCurrent);
  }

  std::vector<std::shared_ptr<Toast>> m_queue;
};
