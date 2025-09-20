#pragma once
#include <qobject.h>
#include <qtmetamacros.h>

class SettingsController : public QObject {
  Q_OBJECT

signals:
  void windowVisiblityChangeRequested(bool value) const;
  void openExtensionPreferencesRequested(const QString &id) const;
  void tabIdOpened(const QString &id) const;

public:
  void openWindow() const;
  void closeWindow() const;
  void openExtensionPreferences(const QString &id);
  void openTab(const QString &tabId);

  SettingsController();
};
