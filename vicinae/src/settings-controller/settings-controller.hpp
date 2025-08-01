#pragma once
#include <qobject.h>
#include <qtmetamacros.h>

class SettingsController : public QObject {
  Q_OBJECT

public:
  void openWindow() const;
  void closeWindow() const;
  void openExtensionPreferences(const QString &id);
  void openTab(const QString &tabId);

  SettingsController();

signals:
  void windowVisiblityChangeRequested(bool value) const;
  void tabIdOpened(const QString &id) const;
};
