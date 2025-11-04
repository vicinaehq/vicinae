#include "wayland.hpp"
#include "environment.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

#include <qscreen.h>
#include <wayland-client-protocol.h>
#include "wayland/virtual-keyboard.hpp"
#include "wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

// Events
static void foreign_toplevel_handle_title(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                          const char *title) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  self->m_title = QString(title);
}

static void foreign_toplevel_handle_app_id(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                           const char *app_id) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  self->m_wmClass = QString(app_id);
  self->m_id = QString::number((uint64_t)(self));
}

static void foreign_toplevel_handle_output_enter(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                                 struct wl_output *output) {
  // ignore
}

static void foreign_toplevel_handle_output_leave(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                                 struct wl_output *output) {
  // ignore
}

static void foreign_toplevel_handle_state(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                          struct wl_array *value) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);

  self->m_active = false;
  size_t n = value->size / sizeof(uint32_t);
  for (size_t i = 0; i < n; ++i) {
    uint32_t *elem = (uint32_t *)value->data + i;
    if (*elem == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED) { self->m_active = true; }
  }
}

static void foreign_toplevel_handle_done(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  emit self->m_manager->windowsChanged();
}

static void foreign_toplevel_handle_closed(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  for (auto iter = self->m_manager->m_toplevels.begin(); iter != self->m_manager->m_toplevels.end(); ++iter) {
    if (iter->get() == self) {
      self->m_manager->m_toplevels.erase(iter);
      break;
    }
  }

  // the handle is inert and will receive no further events
  emit self->m_manager->windowsChanged();
}

static void foreign_toplevel_handle_parent(void *data, struct zwlr_foreign_toplevel_handle_v1 *handle,
                                           struct zwlr_foreign_toplevel_handle_v1 *parent) {
  // ignore
}

static struct zwlr_foreign_toplevel_handle_v1_listener foreign_toplevel_handle_listener = {
    .title = &foreign_toplevel_handle_title,
    .app_id = &foreign_toplevel_handle_app_id,
    .output_enter = &foreign_toplevel_handle_output_enter,
    .output_leave = &foreign_toplevel_handle_output_leave,
    .state = &foreign_toplevel_handle_state,
    .done = &foreign_toplevel_handle_done,
    .closed = &foreign_toplevel_handle_closed,
    .parent = &foreign_toplevel_handle_parent};

WaylandWindow::WaylandWindow(WaylandWindowManager *wm, struct zwlr_foreign_toplevel_handle_v1 *handle) {
  m_manager = wm;
  m_handle = handle;
  m_pid = 0;
  m_active = false;
  zwlr_foreign_toplevel_handle_v1_add_listener(handle, &foreign_toplevel_handle_listener, this);
}

WaylandWindow::~WaylandWindow() {
  if (m_handle) {
    zwlr_foreign_toplevel_handle_v1_destroy(m_handle);
    m_handle = nullptr;
  }
}

QString WaylandWindowManager::id() const { return "wayland"; }
QString WaylandWindowManager::displayName() const { return "Wayland"; }

AbstractWindowManager::WindowList WaylandWindowManager::listWindowsSync() const { return m_toplevels; }

AbstractWindowManager::WindowPtr WaylandWindowManager::getFocusedWindowSync() const {
  for (auto window : m_toplevels) {
    WaylandWindow *ww = static_cast<WaylandWindow *>(window.get());
    if (ww->m_active) { return window; }
  }
  return nullptr;
}

bool WaylandWindowManager::pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) {
  using VK = Wayland::VirtualKeyboard;

  if (!m_keyboard.isAvailable()) { return false; }

  if (app && app->isTerminalEmulator()) {
    return m_keyboard.sendKeySequence(XKB_KEY_V, VK::MOD_CTRL | VK::MOD_SHIFT);
  }

  return m_keyboard.sendKeySequence(XKB_KEY_V, VK::MOD_CTRL);
}

void WaylandWindowManager::focusWindowSync(const AbstractWindow &window) const {
  const WaylandWindow &ww = static_cast<const WaylandWindow &>(window);
  zwlr_foreign_toplevel_handle_v1_activate(ww.m_handle, m_seat);
}

bool WaylandWindowManager::closeWindow(const AbstractWindow &window) const {
  const WaylandWindow &ww = static_cast<const WaylandWindow &>(window);
  zwlr_foreign_toplevel_handle_v1_close(ww.m_handle);

  return true;
}

bool WaylandWindowManager::supportsPaste() const { return m_keyboard.isAvailable(); }

// cosmic needs its own top level management protocol integration
bool WaylandWindowManager::isActivatable() const {
  return QGuiApplication::platformName() == "wayland" && !Environment::isCosmicDesktop();
}

bool WaylandWindowManager::ping() const { return m_manager != nullptr; }

static void foreign_toplevel_manager_toplevel(void *data, struct zwlr_foreign_toplevel_manager_v1 *manager,
                                              struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
  WaylandWindowManager *wm = static_cast<WaylandWindowManager *>(data);

  WaylandWindow *window = new WaylandWindow(wm, toplevel);
  wm->m_toplevels.push_back(AbstractWindowManager::WindowPtr(window));
}

static void foreign_toplevel_manager_finished(void *data, struct zwlr_foreign_toplevel_manager_v1 *manager) {
  WaylandWindowManager *wm = static_cast<WaylandWindowManager *>(data);
  zwlr_foreign_toplevel_manager_v1_destroy(manager);
  wm->m_manager = nullptr;
}

static struct zwlr_foreign_toplevel_manager_v1_listener foreign_toplevel_manager_listener = {
    .toplevel = &foreign_toplevel_manager_toplevel, .finished = &foreign_toplevel_manager_finished};

const uint32_t WLR_FOREIGN_TOPLEVEL_VERSION = 3;

static void handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                          uint32_t version) {
  WaylandWindowManager *wm = static_cast<WaylandWindowManager *>(data);

  if (strcmp(interface, wl_output_interface.name) == 0) {
    wl_output *output =
        static_cast<wl_output *>(wl_registry_bind(registry, name, &wl_output_interface, version));
  }

  if (strcmp(interface, zwlr_foreign_toplevel_manager_v1_interface.name) == 0) {
    wm->m_manager = (struct zwlr_foreign_toplevel_manager_v1 *)wl_registry_bind(
        wm->m_registry, name, &zwlr_foreign_toplevel_manager_v1_interface,
        std::min(version, WLR_FOREIGN_TOPLEVEL_VERSION));
  }
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

static struct wl_registry_listener registry_listener = {.global = &handle_global,
                                                        .global_remove = &handle_global_remove};

void WaylandWindowManager::start() {
  m_display = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display();
  m_seat = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->seat();

  m_registry = wl_display_get_registry(m_display);
  wl_registry_add_listener(m_registry, &registry_listener, this);
  wl_display_roundtrip(m_display);

  if (m_manager == nullptr) {
    // Wayland compositor does not support wlr-foreign-toplevel-management protocol
    return;
  }

  zwlr_foreign_toplevel_manager_v1_add_listener(m_manager, &foreign_toplevel_manager_listener, this);
  // fetch initial set of windows
  wl_display_roundtrip(m_display);
}
