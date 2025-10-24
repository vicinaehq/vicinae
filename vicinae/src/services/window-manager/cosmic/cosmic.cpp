#include "cosmic.hpp"
#include "environment.hpp"
#include <QGuiApplication>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "ext-foreign-toplevel-list-v1-client-protocol.h"
#include "cosmic-toplevel-info-unstable-v1-client-protocol.h"
#include "cosmic-toplevel-management-unstable-v1-client-protocol.h"

// Forward declarations for workspace protocols we don't use yet
struct zcosmic_workspace_handle_v1;
struct ext_workspace_handle_v1;

// Protocol version constants
#define EXT_FOREIGN_TOPLEVEL_VERSION 1
#define COSMIC_TOPLEVEL_INFO_VERSION 3
#define COSMIC_TOPLEVEL_MANAGER_VERSION 4

// ============================================================================
// CosmicWindow - Event Handlers for zcosmic_toplevel_handle_v1
// ============================================================================

static void cosmic_toplevel_handle_title(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                         const char *title) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_title = QString::fromUtf8(title);
}

static void cosmic_toplevel_handle_app_id(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                          const char *app_id) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_wmClass = QString::fromUtf8(app_id);
}

static void cosmic_toplevel_handle_output_enter(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                struct wl_output *output) {
  // Not needed for basic window management
}

static void cosmic_toplevel_handle_output_leave(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                struct wl_output *output) {
  // Not needed for basic window management
}

static void cosmic_toplevel_handle_workspace_enter(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                   struct zcosmic_workspace_handle_v1 *workspace) {
  // Workspace support can be added later
}

static void cosmic_toplevel_handle_workspace_leave(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                   struct zcosmic_workspace_handle_v1 *workspace) {
  // Workspace support can be added later
}

static void cosmic_toplevel_handle_state(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                         struct wl_array *state) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);

  // Reset all state flags
  self->m_active = false;
  self->m_maximized = false;
  self->m_minimized = false;
  self->m_fullscreen = false;
  self->m_sticky = false;

  uint32_t *entry = nullptr;
  for (entry = static_cast<uint32_t *>(state->data);
       reinterpret_cast<char *>(entry) < static_cast<char *>(state->data) + state->size; entry++) {
    switch (*entry) {
    case ZCOSMIC_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED:
      self->m_active = true;
      break;
    case ZCOSMIC_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED:
      self->m_maximized = true;
      break;
    case ZCOSMIC_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED:
      self->m_minimized = true;
      break;
    case ZCOSMIC_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN:
      self->m_fullscreen = true;
      break;
    case ZCOSMIC_TOPLEVEL_HANDLE_V1_STATE_STICKY:
      self->m_sticky = true;
      break;
    }
  }
}

static void cosmic_toplevel_handle_done(void *data, struct zcosmic_toplevel_handle_v1 *handle) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  emit self->m_manager->windowsChanged();
}

static void cosmic_toplevel_handle_closed(void *data, struct zcosmic_toplevel_handle_v1 *handle) {
  // This is deprecated in v2+; we handle closure via ext_foreign_toplevel_handle_v1
}

static void cosmic_toplevel_handle_geometry(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                            struct wl_output *output, int32_t x, int32_t y, int32_t width,
                                            int32_t height) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_geometry.x = x;
  self->m_geometry.y = y;
  self->m_geometry.width = width;
  self->m_geometry.height = height;
  self->m_geometry.valid = true;
}

static void cosmic_toplevel_handle_ext_workspace_enter(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                       struct ext_workspace_handle_v1 *workspace) {
  // Workspace support can be added later
}

static void cosmic_toplevel_handle_ext_workspace_leave(void *data, struct zcosmic_toplevel_handle_v1 *handle,
                                                       struct ext_workspace_handle_v1 *workspace) {
  // Workspace support can be added later
}

static const struct zcosmic_toplevel_handle_v1_listener cosmic_toplevel_handle_listener = {
    .closed = &cosmic_toplevel_handle_closed,
    .done = &cosmic_toplevel_handle_done,
    .title = &cosmic_toplevel_handle_title,
    .app_id = &cosmic_toplevel_handle_app_id,
    .output_enter = &cosmic_toplevel_handle_output_enter,
    .output_leave = &cosmic_toplevel_handle_output_leave,
    .workspace_enter = &cosmic_toplevel_handle_workspace_enter,
    .workspace_leave = &cosmic_toplevel_handle_workspace_leave,
    .state = &cosmic_toplevel_handle_state,
    .geometry = &cosmic_toplevel_handle_geometry,
    .ext_workspace_enter = &cosmic_toplevel_handle_ext_workspace_enter,
    .ext_workspace_leave = &cosmic_toplevel_handle_ext_workspace_leave,
};

// ============================================================================
// CosmicWindow - Event Handlers for ext_foreign_toplevel_handle_v1
// ============================================================================

static void foreign_toplevel_handle_title(void *data, struct ext_foreign_toplevel_handle_v1 *handle,
                                          const char *title) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_title = QString::fromUtf8(title);
}

static void foreign_toplevel_handle_app_id(void *data, struct ext_foreign_toplevel_handle_v1 *handle,
                                           const char *app_id) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_wmClass = QString::fromUtf8(app_id);
}

static void foreign_toplevel_handle_identifier(void *data, struct ext_foreign_toplevel_handle_v1 *handle,
                                               const char *identifier) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  self->m_identifier = QString::fromUtf8(identifier);
  self->m_id = self->m_identifier; // Use stable identifier as window ID
}

static void foreign_toplevel_handle_done(void *data, struct ext_foreign_toplevel_handle_v1 *handle) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);
  emit self->m_manager->windowsChanged();
}

static void foreign_toplevel_handle_closed(void *data, struct ext_foreign_toplevel_handle_v1 *handle) {
  CosmicWindow *self = static_cast<CosmicWindow *>(data);

  // Remove from window list
  for (auto iter = self->m_manager->m_toplevels.begin(); iter != self->m_manager->m_toplevels.end(); ++iter) {
    if (iter->get() == self) {
      self->m_manager->m_toplevels.erase(iter);
      break;
    }
  }

  emit self->m_manager->windowsChanged();
}

static const struct ext_foreign_toplevel_handle_v1_listener foreign_toplevel_handle_listener = {
    .closed = &foreign_toplevel_handle_closed,
    .done = &foreign_toplevel_handle_done,
    .title = &foreign_toplevel_handle_title,
    .app_id = &foreign_toplevel_handle_app_id,
    .identifier = &foreign_toplevel_handle_identifier,
};

// ============================================================================
// CosmicWindow - Constructor & Destructor
// ============================================================================

CosmicWindow::CosmicWindow(CosmicWindowManager *manager,
                           struct ext_foreign_toplevel_handle_v1 *foreign_handle,
                           struct zcosmic_toplevel_handle_v1 *cosmic_handle)
    : m_foreign_handle(foreign_handle), m_cosmic_handle(cosmic_handle), m_manager(manager) {

  // Add listeners to both handles
  ext_foreign_toplevel_handle_v1_add_listener(m_foreign_handle, &foreign_toplevel_handle_listener, this);
  zcosmic_toplevel_handle_v1_add_listener(m_cosmic_handle, &cosmic_toplevel_handle_listener, this);
}

CosmicWindow::~CosmicWindow() {
  if (m_cosmic_handle) { zcosmic_toplevel_handle_v1_destroy(m_cosmic_handle); }
  if (m_foreign_handle) { ext_foreign_toplevel_handle_v1_destroy(m_foreign_handle); }
}

std::optional<AbstractWindowManager::WindowBounds> CosmicWindow::bounds() const {
  if (!m_geometry.valid) { return std::nullopt; }
  return AbstractWindowManager::WindowBounds{.x = static_cast<uint32_t>(m_geometry.x),
                                             .y = static_cast<uint32_t>(m_geometry.y),
                                             .width = static_cast<uint32_t>(m_geometry.width),
                                             .height = static_cast<uint32_t>(m_geometry.height)};
}

// ============================================================================
// CosmicWindowManager - Window Operations
// ============================================================================

AbstractWindowManager::WindowList CosmicWindowManager::listWindowsSync() const { return m_toplevels; }

AbstractWindowManager::WindowPtr CosmicWindowManager::getFocusedWindowSync() const {
  for (const auto &window : m_toplevels) {
    CosmicWindow *cw = static_cast<CosmicWindow *>(window.get());
    if (cw->m_active) { return window; }
  }
  return nullptr;
}

void CosmicWindowManager::focusWindowSync(const AbstractWindow &window) const {
  const CosmicWindow &cw = static_cast<const CosmicWindow &>(window);
  zcosmic_toplevel_manager_v1_activate(m_cosmic_manager, cw.m_cosmic_handle, m_seat);
  wl_display_flush(m_display);
}

bool CosmicWindowManager::closeWindow(const AbstractWindow &window) const {
  const CosmicWindow &cw = static_cast<const CosmicWindow &>(window);
  zcosmic_toplevel_manager_v1_close(m_cosmic_manager, cw.m_cosmic_handle);
  wl_display_flush(m_display);
  return true;
}

bool CosmicWindowManager::pasteToWindow(const AbstractWindow *window, const AbstractApplication *app) {
  using VK = Wayland::VirtualKeyboard;

  if (!m_keyboard.isAvailable()) { return false; }

  if (app && app->isTerminalEmulator()) {
    return m_keyboard.sendKeySequence(XKB_KEY_V, VK::MOD_CTRL | VK::MOD_SHIFT);
  }

  return m_keyboard.sendKeySequence(XKB_KEY_V, VK::MOD_CTRL);
}

// ============================================================================
// CosmicWindowManager - Protocol Event Handlers
// ============================================================================

static void foreign_toplevel_list_toplevel(void *data, struct ext_foreign_toplevel_list_v1 *list,
                                           struct ext_foreign_toplevel_handle_v1 *toplevel) {
  CosmicWindowManager *wm = static_cast<CosmicWindowManager *>(data);

  // Create cosmic toplevel extension for this foreign toplevel
  struct zcosmic_toplevel_handle_v1 *cosmic_toplevel =
      zcosmic_toplevel_info_v1_get_cosmic_toplevel(wm->m_cosmic_info, toplevel);

  // Create window object and add to list
  CosmicWindow *window = new CosmicWindow(wm, toplevel, cosmic_toplevel);
  wm->m_toplevels.push_back(AbstractWindowManager::WindowPtr(window));
}

static void foreign_toplevel_list_finished(void *data, struct ext_foreign_toplevel_list_v1 *list) {
  // Compositor is done with the list
}

static const struct ext_foreign_toplevel_list_v1_listener foreign_toplevel_list_listener = {
    .toplevel = &foreign_toplevel_list_toplevel,
    .finished = &foreign_toplevel_list_finished,
};

static void cosmic_toplevel_info_done(void *data, struct zcosmic_toplevel_info_v1 *info) {
  // All initial toplevel info has been sent
}

static const struct zcosmic_toplevel_info_v1_listener cosmic_toplevel_info_listener = {
    .done = &cosmic_toplevel_info_done,
};

static void cosmic_toplevel_manager_capabilities(void *data, struct zcosmic_toplevel_manager_v1 *manager,
                                                 struct wl_array *capabilities) {
  // Could parse capabilities here if needed for UI
  // For now, we'll just attempt operations and let compositor ignore unsupported ones
}

static const struct zcosmic_toplevel_manager_v1_listener cosmic_toplevel_manager_listener = {
    .capabilities = &cosmic_toplevel_manager_capabilities,
};

// ============================================================================
// CosmicWindowManager - Registry & Initialization
// ============================================================================

static void registry_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
                            uint32_t version) {
  CosmicWindowManager *wm = static_cast<CosmicWindowManager *>(data);

  if (strcmp(interface, ext_foreign_toplevel_list_v1_interface.name) == 0) {
    wm->m_foreign_list = static_cast<struct ext_foreign_toplevel_list_v1 *>(
        wl_registry_bind(registry, name, &ext_foreign_toplevel_list_v1_interface,
                         std::min(version, (uint32_t)EXT_FOREIGN_TOPLEVEL_VERSION)));
  } else if (strcmp(interface, zcosmic_toplevel_info_v1_interface.name) == 0) {
    wm->m_cosmic_info = static_cast<struct zcosmic_toplevel_info_v1 *>(
        wl_registry_bind(registry, name, &zcosmic_toplevel_info_v1_interface,
                         std::min(version, (uint32_t)COSMIC_TOPLEVEL_INFO_VERSION)));
  } else if (strcmp(interface, zcosmic_toplevel_manager_v1_interface.name) == 0) {
    wm->m_cosmic_manager = static_cast<struct zcosmic_toplevel_manager_v1 *>(
        wl_registry_bind(registry, name, &zcosmic_toplevel_manager_v1_interface,
                         std::min(version, (uint32_t)COSMIC_TOPLEVEL_MANAGER_VERSION)));
  }
}

static void registry_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
  // Handle global removal if needed
}

static const struct wl_registry_listener registry_listener = {
    .global = &registry_global,
    .global_remove = &registry_global_remove,
};

// ============================================================================
// CosmicWindowManager - Public Interface
// ============================================================================

CosmicWindowManager::CosmicWindowManager() {}

CosmicWindowManager::~CosmicWindowManager() {
  m_toplevels.clear();

  if (m_cosmic_manager) { zcosmic_toplevel_manager_v1_destroy(m_cosmic_manager); }
  if (m_cosmic_info) { zcosmic_toplevel_info_v1_destroy(m_cosmic_info); }
  if (m_foreign_list) {
    ext_foreign_toplevel_list_v1_stop(m_foreign_list);
    ext_foreign_toplevel_list_v1_destroy(m_foreign_list);
  }
  if (m_registry) { wl_registry_destroy(m_registry); }
}

bool CosmicWindowManager::isActivatable() const {
  return QGuiApplication::platformName() == "wayland" && Environment::isCosmicDesktop();
}

void CosmicWindowManager::start() {
  // Get Wayland display and seat from Qt
  auto *native = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (!native) { return; }

  m_display = native->display();
  m_seat = native->seat();

  if (!m_display) { return; }

  // Register to global events
  m_registry = wl_display_get_registry(m_display);
  wl_registry_add_listener(m_registry, &registry_listener, this);
  wl_display_roundtrip(m_display);

  // Check if all required protocols are available
  if (!m_foreign_list || !m_cosmic_info || !m_cosmic_manager) { return; }

  // Add listeners
  ext_foreign_toplevel_list_v1_add_listener(m_foreign_list, &foreign_toplevel_list_listener, this);
  zcosmic_toplevel_info_v1_add_listener(m_cosmic_info, &cosmic_toplevel_info_listener, this);
  zcosmic_toplevel_manager_v1_add_listener(m_cosmic_manager, &cosmic_toplevel_manager_listener, this);

  // Fetch initial window list
  wl_display_roundtrip(m_display);
}
