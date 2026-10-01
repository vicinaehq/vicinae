#pragma once
#include <memory>
#include <qwindow.h>
#include <unordered_map>
#include <QtWaylandClient/QWaylandClientExtension>
#include "qwayland-ext-background-effect-v1.h"
#include "services/window-material/window-material-backend.hpp"

class ExtBackgroundEffectV1 : public QWaylandClientExtensionTemplate<ExtBackgroundEffectV1>,
                              public QtWayland::ext_background_effect_manager_v1 {
  Q_OBJECT

public:
  ExtBackgroundEffectV1();

  bool supportsBlur() const { return m_capabilities & capability_blur; }

protected:
  void ext_background_effect_manager_v1_capabilities(uint32_t flags) override;

private:
  uint32_t m_capabilities = 0;
};

class ExtBackgroundEffectV1Manager : public WindowMaterialBackend {
public:
  bool isSupported() const override;
  bool apply(QWindow *win, const Params &params) override;
  bool clear(QWindow *win) override;

protected:
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  struct BlurState {
    QtWayland::ext_background_effect_surface_v1 effect;
    Params cfg;

    BlurState(::ext_background_effect_surface_v1 *effect, const Params &cfg) : effect(effect), cfg(cfg) {}
    ~BlurState() {
      if (effect.isInitialized()) effect.destroy();
    }
  };

  void applyBlur(QWindow *win, BlurState &state);

  ExtBackgroundEffectV1 m_manager;
  std::unordered_map<QWindow *, std::unique_ptr<BlurState>> m_state;
};
