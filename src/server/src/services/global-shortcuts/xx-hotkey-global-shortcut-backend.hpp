#pragma once
#include <QCoreApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include <cstdint>
#include <expected>
#include <memory>
#include <variant>
#include <vector>
#include "qwayland-xx-hotkey-v1.h"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class XxHotkeyManagerV1 : public QWaylandClientExtensionTemplate<XxHotkeyManagerV1>,
                          public QtWayland::xx_hotkey_manager_v1 {
  Q_OBJECT

public:
  XxHotkeyManagerV1();
};

class XxHotkeyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_DECLARE_TR_FUNCTIONS(XxHotkeyGlobalShortcutBackend)

public:
  struct KeyTrigger {
    std::uint32_t keysym;
    std::uint32_t modifiers;
  };

  /// Button code in the wl_pointer.button space. Not reachable from the recorder yet.
  struct ButtonTrigger {
    std::uint32_t button;
    std::uint32_t modifiers;
  };

  using Trigger = std::variant<KeyTrigger, ButtonTrigger>;

  XxHotkeyGlobalShortcutBackend();
  ~XxHotkeyGlobalShortcutBackend() override;

  QString id() const override;
  bool start() override;

  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  bool isSupported() const override { return m_manager.isActive(); }

private:
  class Hotkey : public QtWayland::xx_hotkey_v1 {
  public:
    enum class State : std::uint8_t { Pending, Bound, Denied };

    Hotkey(XxHotkeyGlobalShortcutBackend *backend, struct ::xx_hotkey_v1 *object, QString id);
    ~Hotkey() override;

    void apply(const Trigger &trigger, const QString &description);

    QString m_id;
    State m_state = State::Pending;
    QString m_message;

  protected:
    void xx_hotkey_v1_bound() override;
    void xx_hotkey_v1_denied(uint32_t reason, const QString &message) override;
    void xx_hotkey_v1_revoked(const QString &message) override;
    void xx_hotkey_v1_triggered(uint32_t serial, uint32_t time) override;
    void xx_hotkey_v1_released(uint32_t serial, uint32_t time) override;

  private:
    XxHotkeyGlobalShortcutBackend *m_backend;
  };

  std::expected<void, QString> bind(const QString &id, const Trigger &trigger, const QString &description);
  bool isTracked(const Hotkey *hotkey) const;
  void dropHotkey(Hotkey *hotkey);

  XxHotkeyManagerV1 m_manager;
  std::vector<std::unique_ptr<Hotkey>> m_binds;
  Hotkey *m_pendingBind = nullptr;
};
