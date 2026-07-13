#pragma once
#include <expected>
#include <unordered_map>
#include <QAbstractNativeEventFilter>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class WindowsGlobalShortcutBackend : public AbstractGlobalShortcutBackend, public QAbstractNativeEventFilter {
  Q_OBJECT

public:
  WindowsGlobalShortcutBackend() = default;
  ~WindowsGlobalShortcutBackend() override;

  QString id() const override { return "win32-hotkey"; }

  bool start() override;
  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;

  bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
  std::unordered_map<QString, int> m_hotkeyIds;
  std::unordered_map<int, QString> m_idByHotkeyId;
  int m_nextHotkeyId = 1;
  bool m_started = false;
};
