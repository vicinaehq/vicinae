#include "kde-background-effect-manager.hpp"
#include "services/background-effect/abstract-background-effect-manager.hpp"
#include "services/background-effect/dummy-background-effect-manager.hpp"
#include "wayland/globals.hpp"

class BackgroundEffectManagerFactory {
public:
  static std::unique_ptr<AbstractBackgroundEffectManager> create() {
    if (Wayland::Globals::kwinBlur()) { return std::make_unique<KDE::BackgroundEffectManager>(); }
    return std::make_unique<DummyBackgroundEffectManager>();
  }
};
