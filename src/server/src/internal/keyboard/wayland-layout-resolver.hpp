#pragma once
#include "xkb-layout-resolver.hpp"
#include <cstdint>

struct wl_keyboard;

namespace Keyboard {

class WaylandLayoutResolver : public XkbLayoutResolver {
public:
  WaylandLayoutResolver();
  ~WaylandLayoutResolver() override;

  void handleKeymap(uint32_t format, int fd, uint32_t size);
  void handleModifiers(uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group);

private:
  wl_keyboard *m_keyboard = nullptr;
};

} // namespace Keyboard
