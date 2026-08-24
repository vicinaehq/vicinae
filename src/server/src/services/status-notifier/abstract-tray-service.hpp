#pragma once
#include <QFuture>
#include <QObject>
#include <vector>
#include "services/status-notifier/tray-item.hpp"

class AbstractTrayService : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  virtual QString id() const = 0;
  virtual bool isAvailable() const = 0;
  virtual std::vector<TrayItem> items() const = 0;

  virtual void activate(const TrayItem &item, int x = 0, int y = 0) = 0;
  virtual void secondaryActivate(const TrayItem &item, int x = 0, int y = 0) = 0;
  virtual void contextMenu(const TrayItem &item, int x = 0, int y = 0) = 0;
  virtual void scroll(const TrayItem &item, int delta, bool horizontal = false) = 0;

  virtual QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &item) = 0;
  virtual QFuture<std::vector<TrayEntry>> snapshot() = 0;
  virtual void triggerMenuItem(const TrayItem &item, int menuItemId) = 0;

signals:
  void availabilityChanged(bool available);
  void itemAdded(const TrayItem &item);
  void itemChanged(const TrayItem &item);
  void itemRemoved(const QString &key);
  void menuChanged(const TrayItem &item);
  void changed();
};
