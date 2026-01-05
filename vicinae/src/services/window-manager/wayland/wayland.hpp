#pragma once
#include "services/window-manager/abstract-window-manager.hpp"

class WaylandWindowManager;

class WaylandWindow : public AbstractWindowManager::AbstractWindow {
public:
  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }

  // Extended AbstractWindow interface - use defaults for now
  std::optional<QString> workspace() const override { return std::nullopt; }
  bool canClose() const override { return true; }

  WaylandWindow(WaylandWindowManager *manager, struct zwlr_foreign_toplevel_handle_v1 *handle);
  ~WaylandWindow();

  QString m_id;
  QString m_title;
  QString m_wmClass;
  int m_pid;
  bool m_active;
  struct zwlr_foreign_toplevel_handle_v1 *m_handle;
  WaylandWindowManager *m_manager;
};

class WaylandWindowManager : public AbstractWindowManager {
public:
  QString id() const override;
  QString displayName() const override;
  WindowList listWindowsSync() const override;

  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;

  bool supportsPaste() const override;
  bool pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) override;
  bool ping() const override;
  bool isActivatable() const override;

  void start() override;

  ~WaylandWindowManager() override = default;

  struct wl_registry *m_registry;
  struct zwlr_foreign_toplevel_manager_v1 *m_manager;
  WindowList m_toplevels;

private:
  struct wl_display *m_display;
  struct wl_seat *m_seat;
};
