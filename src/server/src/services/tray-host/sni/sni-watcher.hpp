#pragma once
#include <QDBusContext>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QStringList>
#include <QTimer>

/**
 * Minimal org.kde.StatusNotifierWatcher implementation, used only when no other
 * watcher (bar, DE shell) owns the name on the session bus.
 *
 * The name is claimed after a grace period so that a desktop host that briefly drops it
 * (GNOME toggles extensions on lock/unlock) can take it back first, and it is released
 * as soon as another connection is queued for it so the real host wins on its own.
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
  static constexpr int CLAIM_DELAY_MS = 3000;
  static constexpr int QUEUE_POLL_MS = 5000;

  void onNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
  void tryClaim();
  void release();
  void dropped();
  void pollQueuedOwners();
  void serviceGone(const QString &name);

  QDBusServiceWatcher m_nameWatcher;
  QDBusServiceWatcher m_serviceWatcher;
  QTimer m_claimTimer;
  QTimer m_queuePoll;
  QStringList m_items;
  QStringList m_hosts;
  bool m_owned = false;
};
