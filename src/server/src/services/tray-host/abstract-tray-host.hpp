#pragma once
#include <QFuture>
#include <QObject>
#include <vector>
#include "services/tray-host/tray-item.hpp"

class AbstractTrayHost : public QObject {
  Q_OBJECT

public:
  using QObject::QObject;

  virtual QString id() const = 0;
  virtual bool isAvailable() const = 0;
  virtual std::vector<TrayItem> items() const = 0;

  virtual void activate(const TrayItem &item) = 0;
  virtual void secondaryActivate(const TrayItem &item) = 0;
  virtual QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &item) = 0;
  virtual void triggerMenuItem(const TrayItem &item, int menuItemId) = 0;

signals:
  void itemsChanged();
  void menuChanged(const QString &itemKey);
};
