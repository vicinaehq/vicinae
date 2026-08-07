#pragma once
#include <QCoreApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include <expected>
#include <memory>
#include <optional>
#include <vector>
#include "qwayland-vicinae-hotkey-v1.h"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class VicinaeHotkeyManagerV1 : public QWaylandClientExtensionTemplate<VicinaeHotkeyManagerV1>,
                               public QtWayland::vicinae_hotkey_manager_v1 {
  Q_OBJECT

public:
  using QtWayland::vicinae_hotkey_manager_v1::bind;
  using QWaylandClientExtension::bind;

  VicinaeHotkeyManagerV1();
};

class VicinaeHotkeyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_DECLARE_TR_FUNCTIONS(VicinaeHotkeyGlobalShortcutBackend)

public:
  ~VicinaeHotkeyGlobalShortcutBackend() override;

  QString id() const override;
  bool start() override;

  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  void unbindAll() override;
  bool isSupported() const override { return m_manager.isActive(); }

private:
  class Hotkey : public QtWayland::vicinae_hotkey_v1 {
  public:
    Hotkey(VicinaeHotkeyGlobalShortcutBackend *backend, struct ::vicinae_hotkey_v1 *object, QString id);
    ~Hotkey() override;

    QString m_id;

  protected:
    void vicinae_hotkey_v1_denied(uint32_t reason, const QString &message) override;
    void vicinae_hotkey_v1_revoked(uint32_t reason, const QString &message) override;
    void vicinae_hotkey_v1_pressed(uint32_t serial, uint32_t time) override;

  private:
    VicinaeHotkeyGlobalShortcutBackend *m_backend;
  };

  bool isTracked(const Hotkey *hotkey) const;
  void dropHotkey(Hotkey *hotkey);

  VicinaeHotkeyManagerV1 m_manager;
  std::vector<std::unique_ptr<Hotkey>> m_binds;
  Hotkey *m_pendingBind = nullptr;
  std::optional<QString> m_pendingDeny;
};
