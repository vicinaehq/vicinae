#pragma once
#include <QDBusMessage>
#include <QDBusServiceWatcher>
#include <map>
#include <set>
#include "services/tray-host/abstract-tray-host.hpp"
#include "services/tray-host/sni/sni-types.hpp"
#include "services/tray-host/sni/sni-watcher.hpp"

/**
 * StatusNotifierItem tray backend.
 * https://www.freedesktop.org/wiki/Specifications/StatusNotifierItem/
 * Menus are exposed through com.canonical.dbusmenu.
 */
class SniTrayHost : public AbstractTrayHost {
  Q_OBJECT

public:
  SniTrayHost();
  ~SniTrayHost() override;

  QString id() const override { return "sni"; }
  bool isAvailable() const override { return m_available; }
  std::vector<TrayItem> items() const override;

  void activate(const TrayItem &item) override;
  void secondaryActivate(const TrayItem &item) override;
  QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &item) override;
  void triggerMenuItem(const TrayItem &item, int menuItemId) override;

private slots:
  void onItemRegistered(const QString &ref);
  void onItemUnregistered(const QString &ref);
  void onItemSignal(const QDBusMessage &msg);
  void onMenuSignal(const QDBusMessage &msg);

private:
  struct ItemRef {
    QString busName;
    QString path;
  };

  static ItemRef parseItemRef(const QString &ref);

  void watcherAppeared();
  void watcherVanished();
  void registerHost();
  void loadRegisteredItems();
  void addItem(const ItemRef &ref);
  void removeItem(const QString &ref);
  void fetchItem(const ItemRef &ref);
  void resolveOwner(const QString &busName);
  void subscribeItem(const ItemRef &ref, bool subscribe);
  void subscribeMenu(const QString &busName, const QString &menuPath, bool subscribe);
  bool isSender(const QString &busName, const QString &sender) const;
  void callItem(const TrayItem &item, const QString &method, const QVariantList &args);

  static constexpr const char *WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
  static constexpr const char *WATCHER_PATH = "/StatusNotifierWatcher";
  static constexpr const char *WATCHER_IFACE = "org.kde.StatusNotifierWatcher";
  static constexpr const char *ITEM_IFACE = "org.kde.StatusNotifierItem";
  static constexpr const char *MENU_IFACE = "com.canonical.dbusmenu";
  static constexpr const char *PROPERTIES_IFACE = "org.freedesktop.DBus.Properties";

  QDBusServiceWatcher m_watcher;
  SniWatcher m_ownWatcher;
  QString m_hostName;
  bool m_available = false;
  std::map<QString, TrayItem> m_items;
  std::set<QString> m_pending;
  std::map<QString, QString> m_owners;
  std::map<QString, QString> m_menuSubscriptions;
};
