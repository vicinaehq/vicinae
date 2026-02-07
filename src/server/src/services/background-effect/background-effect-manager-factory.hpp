#include "kde-background-effect-manager.hpp"
#include "services/background-effect/abstract-background-effect-manager.hpp"
#include "services/background-effect/dummy-background-effect-manager.hpp"
#include "services/background-effect/ext-background-effect-v1-manager.hpp"
#include "wayland/globals.hpp"

class BackgroundEffectManagerFactory {
public:
  static std::unique_ptr<AbstractBackgroundEffectManager> create() {
    if (auto manager = Wayland::Globals::extBackgroundEffectManager()) {
      return std::make_unique<ExtBackgroundEffectV1Manager>(manager);
    }
    if (auto blur = Wayland::Globals::kwinBlur()) {
      return std::make_unique<KDE::BackgroundEffectManager>(blur);
    }
    return std::make_unique<DummyBackgroundEffectManager>();
  }
};
