#include "xkb-layout-resolver.hpp"
#include <QChar>
#include <xkbcommon/xkbcommon.h>

namespace Keyboard {

XkbLayoutResolver::XkbLayoutResolver() : m_context(xkb_context_new(XKB_CONTEXT_NO_FLAGS)) {}

XkbLayoutResolver::~XkbLayoutResolver() {
  if (m_state) xkb_state_unref(m_state);
  if (m_keymap) xkb_keymap_unref(m_keymap);
  if (m_context) xkb_context_unref(m_context);
}

void XkbLayoutResolver::setKeymap(xkb_keymap *keymap, xkb_state *state) {
  if (m_state) xkb_state_unref(m_state);
  if (m_keymap) xkb_keymap_unref(m_keymap);
  m_keymap = keymap;
  m_state = state;
}

Qt::Key XkbLayoutResolver::unshift(Qt::Key key, quint32 scanCode) {
  if (!m_keymap || !m_state || !scanCode) return key;

  const xkb_layout_index_t layout = xkb_state_key_get_layout(m_state, scanCode);
  if (layout == XKB_LAYOUT_INVALID) return key;

  const xkb_keysym_t *syms = nullptr;
  if (xkb_keymap_key_get_syms_by_level(m_keymap, scanCode, layout, 0, &syms) != 1) return key;

  const uint32_t codepoint = xkb_keysym_to_utf32(syms[0]);
  if (codepoint == 0 || codepoint >= 0x10000) return key;

  const QChar ch(static_cast<char16_t>(codepoint));
  if (!ch.isPrint() || ch.isSpace()) return key;

  return static_cast<Qt::Key>(ch.toUpper().unicode());
}

} // namespace Keyboard
