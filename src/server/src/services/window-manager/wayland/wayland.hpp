#pragma once
#include <optional>
#include <QtWaylandClient/QWaylandClientExtension>
#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "services/window-manager/abstract-wayland-window-manager.hpp"

class WaylandWindowManager;

class ForeignToplevelManagerV1 : public QWaylandClientExtensionTemplate<ForeignToplevelManagerV1>,
                                 public QtWayland::zwlr_foreign_toplevel_manager_v1 {
  Q_OBJECT

public:
  explicit ForeignToplevelManagerV1(WaylandWindowManager *wm);

  bool isFinished() const { return m_finished; }

protected:
  void zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel) override;
  void zwlr_foreign_toplevel_manager_v1_finished() override;

private:
  WaylandWindowManager *m_wm;
  bool m_finished = false;
};

class WaylandWindow : public AbstractWindowManager::AbstractWindow,
                      public QtWayland::zwlr_foreign_toplevel_handle_v1 {
public:
  WaylandWindow(WaylandWindowManager *manager, struct ::zwlr_foreign_toplevel_handle_v1 *handle);
  ~WaylandWindow() override;

  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return std::nullopt; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }

  // Extended AbstractWindow interface - use defaults for now
  std::optional<QString> workspace() const override { return std::nullopt; }
  bool canClose() const override { return true; }

  bool isActive() const { return m_active; }

protected:
  void zwlr_foreign_toplevel_handle_v1_title(const QString &title) override;
  void zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId) override;
  void zwlr_foreign_toplevel_handle_v1_state(wl_array *state) override;
  void zwlr_foreign_toplevel_handle_v1_done() override;
  void zwlr_foreign_toplevel_handle_v1_closed() override;

private:
  QString m_id;
  QString m_title;
  QString m_wmClass;
  WaylandWindowManager *m_manager;
  bool m_active = false;
};

class WaylandWindowManager : public AbstractWaylandWindowManager {
public:
  QString id() const override;
  QString displayName() const override;
  WindowList listWindowsSync() const override;

  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  bool supportsFocusTracking() const override { return true; }
  bool supportsFocusHandoffDetection() const override { return true; }
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;

  bool ping() const override;
  bool isActivatable() const override;

  void start() override;

  ~WaylandWindowManager() override = default;

  WindowList m_toplevels;

private:
  std::optional<ForeignToplevelManagerV1> m_manager;
  struct wl_seat *m_seat = nullptr;
};
