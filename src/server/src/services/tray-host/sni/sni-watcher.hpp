#pragma once
#include <QDBusContext>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QStringList>

/**
 * Minimal org.kde.StatusNotifierWatcher implementation, used only when no other
 * watcher (bar, DE shell) owns the name on the session bus.
 */
class SniWatcher : public QObject, protected QDBusContext {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierWatcher")
  Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ registeredItems)
  Q_PROPERTY(bool IsStatusNotifierHostRegistered READ isHostRegistered)
  Q_PROPERTY(int ProtocolVersion READ protocolVersion)

public:
  explicit SniWatcher(QObject *parent = nullptr);
  ~SniWatcher() override;

  bool tryClaim();
  void release();
  bool owned() const { return m_owned; }

  QStringList registeredItems() const { return m_items; }
  bool isHostRegistered() const { return !m_hosts.isEmpty(); }
  int protocolVersion() const { return 0; }

public slots:
  void RegisterStatusNotifierItem(const QString &service);
  void RegisterStatusNotifierHost(const QString &service);

signals:
  void StatusNotifierItemRegistered(const QString &service);
  void StatusNotifierItemUnregistered(const QString &service);
  void StatusNotifierHostRegistered();
  void StatusNotifierHostUnregistered();

private:
  void serviceGone(const QString &name);

  QDBusServiceWatcher m_serviceWatcher;
  QStringList m_items;
  QStringList m_hosts;
  bool m_owned = false;
};
