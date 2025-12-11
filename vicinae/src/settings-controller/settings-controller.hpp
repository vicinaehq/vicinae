#pragma once
#include "common.hpp"
#include <qobject.h>
#include <qstandardpaths.h>
#include <qtmetamacros.h>

class SettingsWindow;

class SettingsController : public QObject {
  Q_OBJECT

signals:
  void windowVisiblityChangeRequested(bool value) const;
  void openExtensionPreferencesRequested(const EntrypointId &id) const;
  void tabIdOpened(const QString &id) const;

public:
  SettingsController(ApplicationContext &ctx);
  ~SettingsController();

  void openWindow();
  bool closeWindow(bool destroy = true);
  void openExtensionPreferences(const EntrypointId &id);
  void openTab(const QString &tabId);

private:
  /**
   * Create the settings window if it's not created already.
   * If it already is, this is a no-op.
   */
  void createSettingsWindow();
  void destroySettingsWindow();

  ApplicationContext &m_ctx;
  SettingsWindow *m_window = nullptr;
  bool m_isClosing = false;
};
