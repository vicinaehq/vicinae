#pragma once
#include <qwindow.h>
#include "lib/wayland/globals.hpp"
#include "abstract-background-effect-manager.hpp"
#include "common/types.hpp"
#include "services/background-effect/dummy-background-effect-manager.hpp"
#include "services/background-effect/ext-background-effect-v1-manager.hpp"
#include "services/background-effect/kde-background-effect-manager.hpp"

class BackgroundEffectManager : NonCopyable {
public:
  BackgroundEffectManager() : m_manager(createManager()) {}

  /**
   * Request that the compositor blurs a specific region of the background behind the window.
   */
  bool setBlur(QWindow *win, const AbstractBackgroundEffectManager::BlurConfig &cfg) {
    return m_manager->setBlur(win, cfg);
  }

  bool clearBlur(QWindow *win) { return m_manager->removeBlur(win); }

  bool supportsBlur() const { return m_manager->supportsBlur(); }

private:
  static std::unique_ptr<AbstractBackgroundEffectManager> createManager() {
    if (auto manager = Wayland::Globals::extBackgroundEffectManager()) {
      return std::make_unique<ExtBackgroundEffectV1Manager>(manager);
    }
    if (auto blur = Wayland::Globals::kwinBlur()) {
      return std::make_unique<KDE::BackgroundEffectManager>(blur);
    }
    return std::make_unique<DummyBackgroundEffectManager>();
  }

  std::unique_ptr<AbstractBackgroundEffectManager> m_manager;
};
