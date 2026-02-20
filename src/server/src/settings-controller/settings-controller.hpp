#pragma once
#include <QObject>
#include "common/entrypoint.hpp"
#include "common/context.hpp"

class QmlSettingsWindow;

class SettingsController : public QObject {
  Q_OBJECT

public:
  SettingsController(ApplicationContext &ctx);
  ~SettingsController();

  void openWindow();
  bool closeWindow(bool destroy = true);
  void openExtensionPreferences(const EntrypointId &id);
  void openTab(const QString &tabId);

private:
  void createSettingsWindow();
  void destroySettingsWindow();

  ApplicationContext &m_ctx;
  QmlSettingsWindow *m_window = nullptr;
  bool m_isClosing = false;
};
