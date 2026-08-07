#include "xdg-activation.hpp"
#include <QGuiApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include <qlogging.h>
#include <qwindow.h>
#include <wayland-client-core.h>
#include "qt-wayland-utils.hpp"
#include "qwayland-xdg-activation-v1.h"

namespace {

class XdgActivationV1 : public QWaylandClientExtensionTemplate<XdgActivationV1>,
                        public QtWayland::xdg_activation_v1 {
public:
  XdgActivationV1() : QWaylandClientExtensionTemplate(1) { initialize(); }
};

struct Token : QtWayland::xdg_activation_token_v1 {
  using xdg_activation_token_v1::xdg_activation_token_v1;
  ~Token() override {
    if (isInitialized()) destroy();
  }

  std::optional<QString> value;

protected:
  void xdg_activation_token_v1_done(const QString &token) override { value = token; }
};

XdgActivationV1 *activation() {
  if (QGuiApplication::platformName() != "wayland") return nullptr;

  static XdgActivationV1 instance;
  return instance.isActive() ? &instance : nullptr;
}

std::optional<std::uint32_t> g_pendingSerial;

std::optional<QString> mintToken(XdgActivationV1 &activation, std::uint32_t serial, wl_seat *seat,
                                 wl_surface *surface, const QString &appId) {
  Token token(activation.get_activation_token());

  if (seat) token.set_serial(serial, seat);
  if (surface) token.set_surface(surface);
  if (!appId.isEmpty()) token.set_app_id(appId);
  token.commit();

  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  wl_display_roundtrip(app->display());

  return token.value;
}

wl_seat *inputSeat(QNativeInterface::QWaylandApplication *app) {
  if (auto *seat = app->lastInputSeat()) return seat;
  return app->seat();
}

} // namespace

namespace Wayland::XdgActivation {

bool isSupported() { return activation() != nullptr; }

std::optional<QString> requestLaunchToken(const QString &appId) {
  auto *ext = activation();

  if (!ext) return std::nullopt;

  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  auto *win = QGuiApplication::focusWindow();

  if (!win) {
    qWarning() << "xdg-activation: no focused window while requesting a launch token, was the launcher "
                  "window closed before launching? The token will likely not grant focus.";
  }

  wl_surface *surface = win ? QtWaylandUtils::getWindowSurface(win) : nullptr;

  return mintToken(*ext, app->lastInputSerial(), inputSeat(app), surface, appId);
}

void setPendingSerial(std::uint32_t serial) { g_pendingSerial = serial; }

bool activateWindow(QWindow *window) {
  auto *ext = activation();

  if (!ext || !window) return false;

  auto *surface = QtWaylandUtils::getWindowSurface(window);

  if (!surface) {
    qWarning() << "xdg-activation: no wl_surface for window" << window;
    return false;
  }

  qDebug() << "activating window";

  auto *app = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  auto serial = g_pendingSerial.value_or(app->lastInputSerial());

  g_pendingSerial.reset();

  auto token = mintToken(*ext, serial, inputSeat(app), surface, {});

  if (!token) return false;

  ext->activate(*token, surface);
  return true;
}

} // namespace Wayland::XdgActivation
