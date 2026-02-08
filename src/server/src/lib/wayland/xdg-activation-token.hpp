#include "qt-wayland-utils.hpp"
#include "xdg-activation-v1-client-protocol.h"
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <unistd.h>
#include <unordered_map>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

class XdgActivationTokenService : public QObject {
  using Callback = std::function<void(const char *token)>;

  static void done(void *data, xdg_activation_token_v1 *resource, const char *token) {
    auto service = static_cast<XdgActivationTokenService *>(data);

    if (auto it = service->m_callbacks.find(resource); it != service->m_callbacks.end()) {
      it->second(token);
      xdg_activation_token_v1_destroy(resource);
      service->m_callbacks.erase(it);
    }
  }

  static constexpr const xdg_activation_token_v1_listener listener = {.done =
                                                                          XdgActivationTokenService::done};

  // -- Serial tracking via our own pointer/keyboard proxies --

  static void setSerial(void *data, uint32_t serial) {
    static_cast<XdgActivationTokenService *>(data)->m_lastSerial = serial;
  }

  static void onPointerEnter(void *data, wl_pointer *, uint32_t serial, wl_surface *, wl_fixed_t,
                             wl_fixed_t) {
    setSerial(data, serial);
  }
  static void onPointerButton(void *data, wl_pointer *, uint32_t serial, uint32_t, uint32_t, uint32_t) {
    setSerial(data, serial);
  }
  static void onKeyboardEnter(void *data, wl_keyboard *, uint32_t serial, wl_surface *, wl_array *) {
    setSerial(data, serial);
  }
  static void onKeyboardKey(void *data, wl_keyboard *, uint32_t serial, uint32_t, uint32_t, uint32_t) {
    setSerial(data, serial);
  }
  static void onKeymap(void *, wl_keyboard *, uint32_t, int32_t fd, uint32_t) { close(fd); }

  static constexpr wl_pointer_listener pointerListener = {
      .enter = onPointerEnter,
      .leave = [](void *, wl_pointer *, uint32_t, wl_surface *) {},
      .motion = [](void *, wl_pointer *, uint32_t, wl_fixed_t, wl_fixed_t) {},
      .button = onPointerButton,
      .axis = [](void *, wl_pointer *, uint32_t, uint32_t, wl_fixed_t) {},
      .frame = [](void *, wl_pointer *) {},
      .axis_source = [](void *, wl_pointer *, uint32_t) {},
      .axis_stop = [](void *, wl_pointer *, uint32_t, uint32_t) {},
      .axis_discrete = [](void *, wl_pointer *, uint32_t, int32_t) {},
      .axis_value120 = [](void *, wl_pointer *, uint32_t, int32_t) {},
      .axis_relative_direction = [](void *, wl_pointer *, uint32_t, uint32_t) {},
  };

  static constexpr wl_keyboard_listener keyboardListener = {
      .keymap = onKeymap,
      .enter = onKeyboardEnter,
      .leave = [](void *, wl_keyboard *, uint32_t, wl_surface *) {},
      .key = onKeyboardKey,
      .modifiers = [](void *, wl_keyboard *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {},
      .repeat_info = [](void *, wl_keyboard *, int32_t, int32_t) {},
  };

public:
  XdgActivationTokenService(xdg_activation_v1 *manager) : m_manager(manager) {
    auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!app) return;

    m_seat = app->seat();
    if (!m_seat) return;

    m_pointer = wl_seat_get_pointer(m_seat);
    if (m_pointer) wl_pointer_add_listener(m_pointer, &pointerListener, this);

    m_keyboard = wl_seat_get_keyboard(m_seat);
    if (m_keyboard) wl_keyboard_add_listener(m_keyboard, &keyboardListener, this);
  }

  ~XdgActivationTokenService() {
    if (m_pointer) wl_pointer_destroy(m_pointer);
    if (m_keyboard) wl_keyboard_destroy(m_keyboard);
  }

  void requestToken(QWindow *win, const std::optional<std::string> &appId, const Callback &callback) {
    auto token = xdg_activation_v1_get_activation_token(m_manager);
    auto surface = QtWaylandUtils::getWindowSurface(win);

    xdg_activation_token_v1_set_surface(token, surface);
    xdg_activation_token_v1_add_listener(token, &listener, this);

    if (appId) { xdg_activation_token_v1_set_app_id(token, appId->c_str()); }

    if (m_seat && m_lastSerial > 0) {
      qDebug() << "serial" << m_lastSerial;
      xdg_activation_token_v1_set_serial(token, m_lastSerial, m_seat);
    }

    xdg_activation_token_v1_commit(token);
    m_callbacks[token] = callback;
  }

private:
  std::unordered_map<xdg_activation_token_v1 *, Callback> m_callbacks;
  xdg_activation_v1 *m_manager;
  wl_seat *m_seat = nullptr;
  wl_pointer *m_pointer = nullptr;
  wl_keyboard *m_keyboard = nullptr;
  uint32_t m_lastSerial = 0;
};
