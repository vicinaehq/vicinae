#pragma once
#include <QDBusArgument>
#include <QDBusContext>
#include <QVariantMap>
#include <QDBusServiceWatcher>
#include <map>
#include "services/status-notifier/abstract-tray-service.hpp"
#include "services/status-notifier/sni/sni-watcher.hpp"

struct SniPixmap {
  int width = 0;
  int height = 0;
  QByteArray data;
};

struct SniToolTip {
  QString iconName;
  QList<SniPixmap> pixmaps;
  QString title;
  QString description;
};

struct DBusMenuLayout {
  int id = 0;
  QVariantMap properties;
  QList<DBusMenuLayout> children;
};

Q_DECLARE_METATYPE(SniPixmap)
Q_DECLARE_METATYPE(SniToolTip)
Q_DECLARE_METATYPE(DBusMenuLayout)

QDBusArgument &operator<<(QDBusArgument &arg, const SniPixmap &pixmap);
const QDBusArgument &operator>>(const QDBusArgument &arg, SniPixmap &pixmap);
QDBusArgument &operator<<(QDBusArgument &arg, const SniToolTip &tooltip);
const QDBusArgument &operator>>(const QDBusArgument &arg, SniToolTip &tooltip);
QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuLayout &layout);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuLayout &layout);

/**
 * StatusNotifierItem tray backend.
 * https://www.freedesktop.org/wiki/Specifications/StatusNotifierItem/
 * Menus are exposed through com.canonical.dbusmenu.
 */
class SniTrayService : public AbstractTrayService, protected QDBusContext {
  Q_OBJECT

public:
  SniTrayService();
  ~SniTrayService() override;

  QString id() const override { return "sni"; }
  bool isAvailable() const override { return m_available; }
  std::vector<TrayItem> items() const override;

  void activate(const TrayItem &item, int x, int y) override;
  void secondaryActivate(const TrayItem &item, int x, int y) override;
  void contextMenu(const TrayItem &item, int x, int y) override;
  void scroll(const TrayItem &item, int delta, bool horizontal) override;

  QFuture<std::vector<TrayMenuItem>> menu(const TrayItem &item) override;
  QFuture<std::vector<TrayEntry>> snapshot() override;
  void triggerMenuItem(const TrayItem &item, int menuItemId) override;

private slots:
  void onItemRegistered(const QString &ref);
  void onItemUnregistered(const QString &ref);
  void onItemSignal();
  void onMenuSignal();

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
  std::map<QString, QString> m_menuSubscriptions;
};
