#pragma once
#include "vicinae-hotkey-v1-client-protocol.h"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include <QCoreApplication>
#include <cstdint>
#include <expected>
#include <optional>
#include <qtmetamacros.h>

class VicinaeHotkeyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_DECLARE_TR_FUNCTIONS(VicinaeHotkeyGlobalShortcutBackend)

public:
  ~VicinaeHotkeyGlobalShortcutBackend() override;

  QString id() const override;
  bool start() override;

  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  bool isSupported() const override { return true; }

private:
  struct HotkeyInfo {
    vicinae_hotkey_v1 *handle;
    QString id;
  };

  static void bound(void *data, vicinae_hotkey_v1 *hotkey);

  static void denied(void *data, vicinae_hotkey_v1 *hotkey, uint32_t reason, const char *msg);

  static void revoked(void *data, vicinae_hotkey_v1 *hotkey, uint32_t reason, const char *msg);

  static void pressed(void *data, struct vicinae_hotkey_v1 *vicinae_hotkey_v1, uint32_t serial,
                      uint32_t time);

  static void released(void *data, struct vicinae_hotkey_v1 *vicinae_hotkey_v1, uint32_t serial,
                       uint32_t time);

  static constexpr const vicinae_hotkey_v1_listener listener = {bound, denied, revoked, pressed, released};

  HotkeyInfo *findHotkey(vicinae_hotkey_v1 *hotkey);
  void dropHotkey(vicinae_hotkey_v1 *hotkey);

  std::vector<HotkeyInfo> m_binds;
  vicinae_hotkey_v1 *m_pendingBind = nullptr;
  std::optional<QString> m_pendingDeny;
};
