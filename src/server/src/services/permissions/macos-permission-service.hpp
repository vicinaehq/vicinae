#pragma once
#include <QObject>
#include <QTimer>

class MacosPermissionService : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool accessibilityGranted READ accessibilityGranted NOTIFY accessibilityGrantedChanged)
  Q_PROPERTY(bool fullDiskAccessGranted READ fullDiskAccessGranted NOTIFY fullDiskAccessGrantedChanged)
  Q_PROPERTY(bool notificationsGranted READ notificationsGranted NOTIFY notificationsGrantedChanged)
  Q_PROPERTY(bool notificationsSupported READ notificationsSupported CONSTANT)

signals:
  void accessibilityGrantedChanged();
  void fullDiskAccessGrantedChanged();
  void notificationsGrantedChanged();

public:
  explicit MacosPermissionService(QObject *parent = nullptr);

  bool accessibilityGranted() const { return m_accessibilityGranted; }
  bool fullDiskAccessGranted() const { return m_fullDiskAccessGranted; }
  bool notificationsGranted() const { return m_notificationsGranted; }
  bool notificationsSupported() const { return m_notificationsSupported; }

  void setWatching(bool value);

  Q_INVOKABLE void requestAccessibility();
  Q_INVOKABLE void requestFullDiskAccess();
  Q_INVOKABLE void requestNotifications();

private:
  void refresh();
  void refreshNotifications();

  QTimer m_pollTimer;
  bool m_accessibilityGranted = false;
  bool m_fullDiskAccessGranted = false;
  bool m_notificationsGranted = false;
  bool m_notificationsNotDetermined = false;
  bool m_notificationsSupported = false;
};
