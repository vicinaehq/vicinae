#pragma once
#include "services/tray-host/sni/sni-types.hpp"
#include "services/tray/tray-service.hpp"
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>
#include <QObject>
#include <vector>

class TrayServiceLinux;

class SniItemObject : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierItem")
  Q_PROPERTY(QString Category READ category)
  Q_PROPERTY(QString Id READ id)
  Q_PROPERTY(QString Title READ title)
  Q_PROPERTY(QString Status READ status)
  Q_PROPERTY(QString IconName READ iconName)
  Q_PROPERTY(QList<SniPixmap> IconPixmap READ iconPixmap)
  Q_PROPERTY(QString IconThemePath READ iconThemePath)
  Q_PROPERTY(QString AttentionIconName READ attentionIconName)
  Q_PROPERTY(QList<SniPixmap> AttentionIconPixmap READ attentionIconPixmap)
  Q_PROPERTY(QString OverlayIconName READ overlayIconName)
  Q_PROPERTY(QList<SniPixmap> OverlayIconPixmap READ overlayIconPixmap)
  Q_PROPERTY(SniToolTip ToolTip READ toolTip)
  Q_PROPERTY(bool ItemIsMenu READ itemIsMenu)
  Q_PROPERTY(QDBusObjectPath Menu READ menu)
  Q_PROPERTY(int WindowId READ windowId)

public:
  explicit SniItemObject(TrayServiceLinux &owner);

  QString category() const { return "ApplicationStatus"; }
  QString id() const { return "vicinae"; }
  QString title() const { return "Vicinae"; }
  QString status() const;
  QString iconName() const { return m_iconName; }
  QList<SniPixmap> iconPixmap() const { return m_pixmaps; }
  QString iconThemePath() const { return {}; }
  QString attentionIconName() const { return {}; }
  QList<SniPixmap> attentionIconPixmap() const { return {}; }
  QString overlayIconName() const { return {}; }
  QList<SniPixmap> overlayIconPixmap() const { return {}; }
  SniToolTip toolTip() const;
  bool itemIsMenu() const { return false; }
  QDBusObjectPath menu() const;
  int windowId() const { return 0; }

public slots:
  void Activate(int x, int y);
  void SecondaryActivate(int x, int y);
  void ContextMenu(int x, int y);
  void Scroll(int delta, const QString &orientation);
  void ProvideXdgActivationToken(const QString &token);

signals:
  void NewTitle();
  void NewIcon();
  void NewAttentionIcon();
  void NewOverlayIcon();
  void NewToolTip();
  void NewStatus(const QString &status);
  void NewIconThemePath(const QString &path);
  void NewMenu();

private:
  TrayServiceLinux &m_owner;
  QString m_iconName;
  QList<SniPixmap> m_pixmaps;
};

class DBusMenuObject : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "com.canonical.dbusmenu")
  Q_PROPERTY(uint Version READ version)
  Q_PROPERTY(QString TextDirection READ textDirection)
  Q_PROPERTY(QString Status READ status)
  Q_PROPERTY(QStringList IconThemePath READ iconThemePath)

public:
  explicit DBusMenuObject(TrayServiceLinux &owner);

  uint version() const { return 3; }
  QString textDirection() const { return "ltr"; }
  QString status() const { return "normal"; }
  QStringList iconThemePath() const { return {}; }

  void layoutChanged();

public slots:
  uint GetLayout(int parentId, int recursionDepth, const QStringList &propertyNames, DBusMenuLayout &layout);
  QList<DBusMenuItemProperties> GetGroupProperties(const QList<int> &ids, const QStringList &propertyNames);
  QDBusVariant GetProperty(int id, const QString &name);
  void Event(int id, const QString &eventId, const QDBusVariant &data, uint timestamp);
  QList<int> EventGroup(const QList<DBusMenuEvent> &events);
  bool AboutToShow(int id);
  QList<int> AboutToShowGroup(const QList<int> &ids, QList<int> &idErrors);

signals:
  void ItemsPropertiesUpdated(const QList<DBusMenuItemProperties> &updated,
                              const QList<DBusMenuItemKeys> &removed);
  void LayoutUpdated(uint revision, int parent);
  void ItemActivationRequested(int id, uint timestamp);

private:
  QVariantMap itemProperties(int id, const QStringList &names) const;

  TrayServiceLinux &m_owner;
  uint m_revision = 1;
};

/**
 * Exposes Vicinae as a StatusNotifierItem on the session bus, with its menu served
 * through com.canonical.dbusmenu. Registers with org.kde.StatusNotifierWatcher whenever
 * one is available.
 */
class TrayServiceLinux : public TrayService {
  Q_OBJECT

public:
  struct MenuEntry {
    enum class Kind { Toggle, Version, Separator, About, Settings, Sponsor, Discord, Follow, Quit };

    int id = 0;
    Kind kind = Kind::Toggle;
  };

  static constexpr const char *ITEM_PATH = "/StatusNotifierItem";
  static constexpr const char *MENU_PATH = "/MenuBar";

  explicit TrayServiceLinux(QObject *parent = nullptr);
  ~TrayServiceLinux() override;

  void setVersion(const QString &version) override;
  void setCheckForUpdatesVisible(bool visible) override;
  void setAvailableUpdate(const QString &tag) override;
  void show() override;
  void hide() override;

  bool visible() const { return m_visible; }
  const std::vector<MenuEntry> &entries() const { return m_entries; }
  QString entryLabel(const MenuEntry &entry) const;
  bool entryEnabled(const MenuEntry &entry) const;
  void activateEntry(int id);

private:
  void registerWithWatcher();

  QDBusServiceWatcher m_watcher;
  SniItemObject m_item;
  DBusMenuObject m_menu;
  std::vector<MenuEntry> m_entries;
  QString m_serviceName;
  QString m_version;
  bool m_visible = false;
  bool m_registered = false;
};
