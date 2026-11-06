#include "ui/quick/modal-guard.hpp"
#include <QQuickItem>

void ModalGuard::block(QQuickItem *overlay) {
  if (!overlay) return;
  if (m_depth[overlay]++ == 0) setContentEnabled(overlay, false);
}

void ModalGuard::unblock(QQuickItem *overlay) {
  if (!overlay) return;
  const auto it = m_depth.find(overlay);
  if (it == m_depth.end()) return;
  if (--it->second > 0) return;
  m_depth.erase(it);
  setContentEnabled(overlay, true);
}

void ModalGuard::setContentEnabled(QQuickItem *overlay, bool enabled) {
  auto *host = overlay->parentItem();
  if (!host) return;
  for (auto *child : host->childItems()) {
    if (child != overlay) child->setEnabled(enabled);
  }
}
