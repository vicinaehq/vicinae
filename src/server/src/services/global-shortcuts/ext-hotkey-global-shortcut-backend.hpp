#pragma once
#include "ext-hotkey-v1-client-protocol.h"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include <cstdint>
#include <expected>
#include <optional>
#include <qtmetamacros.h>

class ExtHotkeyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
public:
  ~ExtHotkeyGlobalShortcutBackend() override;

  QString id() const override;
  bool start() override;

  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  bool isSupported() const override { return true; }

private:
  struct HotkeyInfo {
    ext_hotkey_v1 *handle;
    QString id;
  };

  static void bound(void *data, ext_hotkey_v1 *hotkey);

  static void denied(void *data, ext_hotkey_v1 *hotkey, uint32_t reason, const char *msg);

  static void revoked(void *data, ext_hotkey_v1 *hotkey, uint32_t reason, const char *msg);

  static void pressed(void *data, struct ext_hotkey_v1 *ext_hotkey_v1, uint32_t serial, uint32_t time);

  static void released(void *data, struct ext_hotkey_v1 *ext_hotkey_v1, uint32_t serial, uint32_t time);

  static constexpr const ext_hotkey_v1_listener listener = {bound, denied, revoked, pressed, released};

  HotkeyInfo *findHotkey(ext_hotkey_v1 *hotkey);
  void dropHotkey(ext_hotkey_v1 *hotkey);

  std::vector<HotkeyInfo> m_binds;
  ext_hotkey_v1 *m_pendingBind = nullptr;
  std::optional<QString> m_pendingDeny;
};
