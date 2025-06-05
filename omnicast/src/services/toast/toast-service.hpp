#pragma once
#include "ui/toast.hpp"
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

  void close() const { emit destroyRequested(); }

signals:
  void updated() const;
  void destroyRequested() const;
};

class ToastService : public QObject {
  Q_OBJECT

  std::deque<Toast *> m_queue;

  void destroyToast(Toast *toast) {
    if (auto it = std::ranges::find(m_queue, toast); it != m_queue.end()) {
      (*it)->deleteLater();
      m_queue.erase(it);
    }
  }

public:
  void registerToast(Toast *toast) {
    connect(toast, &Toast::destroyRequested, this, [this, toast]() { emit toastDestroyed(toast); });
    connect(toast, &Toast::updated, this, [this, toast]() { emit toastUpdated(toast); });
  }

  void setToast(const QString &title, ToastPriority priority, int duration = 2000) {
    auto toast = new Toast(title, priority);

    QTimer::singleShot(duration, this, [this, toast]() {
      toast->close();
      toast->deleteLater();
    });

    registerToast(toast);
    emit toastUpdated(toast);
  }

signals:
  void toastUpdated(const Toast *toast) const;
  void toastDestroyed(const Toast *toast) const;
};
