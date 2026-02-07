#pragma once
#include <qwindow.h>
#include "abstract-background-effect-manager.hpp"
#include "background-effect-manager-factory.hpp"
#include "common/types.hpp"

class BackgroundEffectManager : NonCopyable {
public:
  BackgroundEffectManager() : m_manager(BackgroundEffectManagerFactory::create()) {}

  /**
   * Request that the compositor blurs a specific region of the background behind the window.
   */
  bool setBlur(QWindow *win, const AbstractBackgroundEffectManager::BlurConfig &cfg) {
    return m_manager->setBlur(win, cfg);
  }

  bool clearBlur(QWindow *win) { return m_manager->removeBlur(win); }

  bool supportsBlur() const { return m_manager->supportsBlur(); }

private:
  std::unique_ptr<AbstractBackgroundEffectManager> m_manager;
};
