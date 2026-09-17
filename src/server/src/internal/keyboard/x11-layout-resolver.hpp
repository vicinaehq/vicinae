#pragma once
#include "xkb-layout-resolver.hpp"
#include <QObject>
#include <QSocketNotifier>
#include <cstdint>

struct xcb_connection_t;

namespace Keyboard {

class X11LayoutResolver : public QObject, public XkbLayoutResolver {
public:
  X11LayoutResolver();
  ~X11LayoutResolver() override;

private:
  void reload();
  void drainEvents();

  xcb_connection_t *m_connection = nullptr;
  int32_t m_device = -1;
  uint8_t m_firstEvent = 0;
  QSocketNotifier *m_notifier = nullptr;
};

} // namespace Keyboard
