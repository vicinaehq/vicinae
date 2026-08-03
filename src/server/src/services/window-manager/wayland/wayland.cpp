#include "wayland.hpp"
#include "environment.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

#include <algorithm>
#include <qguiapplication.h>
#include <ranges>
#include <span>
#include <wayland-client-core.h>

constexpr uint32_t WLR_FOREIGN_TOPLEVEL_VERSION = 3;

ForeignToplevelManagerV1::ForeignToplevelManagerV1(WaylandWindowManager *wm)
    : QWaylandClientExtensionTemplate(WLR_FOREIGN_TOPLEVEL_VERSION), m_wm(wm) {
  initialize();
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_toplevel(
    struct ::zwlr_foreign_toplevel_handle_v1 *toplevel) {
  m_wm->m_toplevels.emplace_back(std::make_shared<WaylandWindow>(m_wm, toplevel));
}

void ForeignToplevelManagerV1::zwlr_foreign_toplevel_manager_v1_finished() {
  // the protocol has no destructor request; the manager is inert once finished is received
  wl_proxy_destroy(reinterpret_cast<wl_proxy *>(object()));
  m_finished = true;
}

WaylandWindow::WaylandWindow(WaylandWindowManager *manager, struct ::zwlr_foreign_toplevel_handle_v1 *handle)
    : QtWayland::zwlr_foreign_toplevel_handle_v1(handle), m_manager(manager) {}

WaylandWindow::~WaylandWindow() {
  if (isInitialized()) destroy();
}

void WaylandWindow::zwlr_foreign_toplevel_handle_v1_title(const QString &title) { m_title = title; }

void WaylandWindow::zwlr_foreign_toplevel_handle_v1_app_id(const QString &appId) {
  m_wmClass = appId;
  m_id = QString::number(reinterpret_cast<quintptr>(this));
}

void WaylandWindow::zwlr_foreign_toplevel_handle_v1_state(wl_array *state) {
  bool const wasActive = m_active;
  auto states = std::span(static_cast<const uint32_t *>(state->data), state->size / sizeof(uint32_t));

  m_active = std::ranges::contains(states, static_cast<uint32_t>(state_activated));

  if (!wasActive && m_active) { emit m_manager->focusChanged(); }
}

void WaylandWindow::zwlr_foreign_toplevel_handle_v1_done() { emit m_manager->windowsChanged(); }

void WaylandWindow::zwlr_foreign_toplevel_handle_v1_closed() {
  // the handle is inert and will receive no further events; erasing destroys *this
  auto *manager = m_manager;
  std::erase_if(manager->m_toplevels, [this](const auto &window) { return window.get() == this; });
  emit manager->windowsChanged();
}

QString WaylandWindowManager::id() const { return "wayland"; }
QString WaylandWindowManager::displayName() const { return "Wayland"; }

AbstractWindowManager::WindowList WaylandWindowManager::listWindowsSync() const { return m_toplevels; }

AbstractWindowManager::WindowPtr WaylandWindowManager::getFocusedWindowSync() const {
  for (const auto &window : m_toplevels) {
    if (auto *ww = dynamic_cast<const WaylandWindow *>(window.get()); ww && ww->isActive()) { return window; }
  }
  return nullptr;
}

void WaylandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  auto it = std::ranges::find_if(m_toplevels, [&](const auto &w) { return w.get() == &window; });
  if (it == m_toplevels.end()) return;

  if (auto *ww = dynamic_cast<WaylandWindow *>(it->get())) { ww->activate(m_seat); }
}

bool WaylandWindowManager::closeWindow(const AbstractWindow &window) const {
  auto it = std::ranges::find_if(m_toplevels, [&](const auto &w) { return w.get() == &window; });
  if (it == m_toplevels.end()) return false;

  if (auto *ww = dynamic_cast<WaylandWindow *>(it->get())) { ww->close(); }

  return true;
}

// cosmic needs its own top level management protocol integration
bool WaylandWindowManager::isActivatable() const {
  return QGuiApplication::platformName() == "wayland" && !Environment::isCosmicDesktop();
}

bool WaylandWindowManager::ping() const {
  return m_manager && m_manager->isInitialized() && !m_manager->isFinished();
}

void WaylandWindowManager::start() {
  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();

  if (!app) return;

  m_seat = app->seat();
  m_manager.emplace(this);

  if (!m_manager->isInitialized()) {
    // Wayland compositor does not support wlr-foreign-toplevel-management protocol
    return;
  }

  // fetch the initial set of windows
  wl_display_roundtrip(app->display());
}
