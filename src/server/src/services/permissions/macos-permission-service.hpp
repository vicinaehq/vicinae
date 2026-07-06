#pragma once
#include <QObject>
#include <QTimer>

class MacosPermissionService : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool accessibilityGranted READ accessibilityGranted NOTIFY accessibilityGrantedChanged)
  Q_PROPERTY(bool fullDiskAccessGranted READ fullDiskAccessGranted NOTIFY fullDiskAccessGrantedChanged)
  Q_PROPERTY(bool watching READ watching WRITE setWatching NOTIFY watchingChanged)

signals:
  void accessibilityGrantedChanged();
  void fullDiskAccessGrantedChanged();
  void watchingChanged();

public:
  explicit MacosPermissionService(QObject *parent = nullptr);

  bool accessibilityGranted() const { return m_accessibilityGranted; }
  bool fullDiskAccessGranted() const { return m_fullDiskAccessGranted; }

  bool watching() const { return m_pollTimer.isActive(); }
  void setWatching(bool value);

  Q_INVOKABLE void requestAccessibility();
  Q_INVOKABLE void requestFullDiskAccess();

private:
  void refresh();

  QTimer m_pollTimer;
  bool m_accessibilityGranted = false;
  bool m_fullDiskAccessGranted = false;
};
