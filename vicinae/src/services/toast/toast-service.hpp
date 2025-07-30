#pragma once
#include "ui/toast/toast.hpp"
#include <deque>
#include <qobject.h>
#include <qstring.h>
#include <qtmetamacros.h>

class Toast : public QObject {
  Q_OBJECT

  QString m_title;
  ToastPriority m_priority;

public:
  Toast(const QString &title, ToastPriority priority) : m_title(title), m_priority(priority) {}

  const QString &title() const { return m_title; }

  ToastPriority priority() const { return m_priority; }

  void setTitle(const QString &title) {
    m_title = title;
    emit updated();
  }

  void setPriority(ToastPriority priority) { m_priority = priority; }

  void update() const { emit updated(); }

  void close() const { emit destroyRequested(); }

  ~Toast() { close(); }

signals:
  void updated() const;
  void destroyRequested() const;
};

class ToastService : public QObject {
  Q_OBJECT

  std::deque<Toast *> m_queue;
  const Toast *m_toast = nullptr;

  void destroyToast(Toast *toast) {
    if (auto it = std::ranges::find(m_queue, toast); it != m_queue.end()) {
      (*it)->deleteLater();
      m_queue.erase(it);
    }
  }

public:
  Toast const *currentToast() { return m_toast; }

  void registerToast(Toast *toast) {
    m_toast = toast;
    emit toastActivated(toast);
    connect(toast, &Toast::destroyRequested, this, [toast, this]() {
      if (m_toast == toast) {
        qDebug() << "got toast destroy!";
        emit toastHidden(toast);
        m_toast = nullptr;
      }
    });
  }

  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success, int duration = 2000) {
    auto toast = new Toast(title, priority);

    QTimer::singleShot(duration, toast, [this, toast]() {
      toast->close();
      toast->deleteLater();
    });

    registerToast(toast);
  }

signals:
  void toastActivated(const Toast *toast) const;
  void toastHidden(const Toast *toast) const;
};
