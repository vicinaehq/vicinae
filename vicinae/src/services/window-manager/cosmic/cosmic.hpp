#pragma once

#include "../abstract-window-manager.hpp"
#include "wayland/virtual-keyboard.hpp"
#include <QObject>

class CosmicWindowManager;

class CosmicWindow : public AbstractWindowManager::AbstractWindow {
public:
  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return std::nullopt; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<QString> workspace() const override { return std::nullopt; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override;
  bool canClose() const override { return true; }

  CosmicWindow(CosmicWindowManager *manager, struct ext_foreign_toplevel_handle_v1 *foreign_handle,
               struct zcosmic_toplevel_handle_v1 *cosmic_handle);
  ~CosmicWindow();

  QString m_id;              // Unique identifier
  QString m_identifier;      // Stable identifier from protocol
  QString m_title;           // Window title
  QString m_wmClass;         // Application ID
  bool m_active = false;     // Active/focused state
  bool m_maximized = false;  // Maximized state
  bool m_minimized = false;  // Minimized state
  bool m_fullscreen = false; // Fullscreen state
  bool m_sticky = false;     // Sticky state

  struct {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    bool valid = false;
  } m_geometry; // Window geometry

  struct ext_foreign_toplevel_handle_v1 *m_foreign_handle; // Base protocol handle
  struct zcosmic_toplevel_handle_v1 *m_cosmic_handle;      // Cosmic extension handle
  CosmicWindowManager *m_manager;

  friend class CosmicWindowManager;
};

class CosmicWindowManager : public AbstractWindowManager {
  Q_OBJECT

public:
  CosmicWindowManager();
  ~CosmicWindowManager();

  QString id() const override { return "cosmic"; }
  QString displayName() const override { return "Cosmic Desktop"; }

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool supportsPaste() const override { return m_keyboard.isAvailable(); }
  bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) override;

  bool ping() const override { return true; }
  bool isActivatable() const override;
  void start() override;

  struct wl_registry *m_registry = nullptr;
  struct wl_display *m_display = nullptr;
  struct wl_seat *m_seat = nullptr;

  struct ext_foreign_toplevel_list_v1 *m_foreign_list = nullptr;
  struct zcosmic_toplevel_info_v1 *m_cosmic_info = nullptr;
  struct zcosmic_toplevel_manager_v1 *m_cosmic_manager = nullptr;

  WindowList m_toplevels;              // Managed windows
  Wayland::VirtualKeyboard m_keyboard; // Virtual keyboard for paste

  friend class CosmicWindow;
};
