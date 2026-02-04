#pragma once
#include "ui/toast/toast.hpp"
#include <qstring.h>
#include "common/qt.hpp"

class Toast : public QObject {
  Q_OBJECT

signals:
  void updated() const;
  void destroyRequested() const;

public:
  Toast(const QString &title, ToastStyle priority, const QString &message)
      : m_title(title), m_priority(priority), m_message(message) {}

  const QString &title() const { return m_title; }
  const QString &message() const { return m_message; }

  ToastStyle priority() const { return m_priority; }

  void setTitle(const QString &title) {
    m_title = title;
    emit updated();
  }

  void setMessage(const QString &message) {
    m_message = message;
    emit updated();
  }

  void setPriority(ToastStyle priority) { m_priority = priority; }

  void update() const { emit updated(); }

  void close() const { emit destroyRequested(); }

  ~Toast() { close(); }

  QString m_title;
  QString m_message;
  ToastStyle m_priority;
};

class ToastService : public QObject {
  Q_OBJECT

signals:
  void toastActivated(const Toast *toast) const;
  void toastHidden() const;

public:
  Toast *currentToast() { return m_queue.empty() ? nullptr : m_queue.back().get(); }

  void success(const QString &title, const QString &message = "") {
    setToast(title, ToastStyle::Success, message);
  }
  void failure(const QString &title, const QString &message = "") {
    setToast(title, ToastStyle::Danger, message);
  }
  void dynamic(const QString &title) { setToast(title, ToastStyle::Dynamic); }

  void clear() {
    m_queue.clear();
    emit toastHidden();
  }

  void setToast(const QString &title, ToastStyle priority = ToastStyle::Success, const QString &message = "",
                int duration = 2000) {
    // for now we only handle one toast and we replace it every time. We will use the stack if we find the
    // need to handle many toasts concurrently.
    clear();
    auto toast = std::shared_ptr<Toast>(new Toast(title, priority, message), QObjectDeleter());

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
