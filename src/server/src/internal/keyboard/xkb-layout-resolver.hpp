#pragma once
#include "layout-resolver.hpp"

struct xkb_context;
struct xkb_keymap;
struct xkb_state;

namespace Keyboard {

class XkbLayoutResolver : public LayoutResolver {
public:
  XkbLayoutResolver();
  ~XkbLayoutResolver() override;

  Qt::Key unshift(Qt::Key key, quint32 scanCode) override;

protected:
  void setKeymap(xkb_keymap *keymap, xkb_state *state);

  xkb_context *m_context = nullptr;
  xkb_keymap *m_keymap = nullptr;
  xkb_state *m_state = nullptr;
};

} // namespace Keyboard
