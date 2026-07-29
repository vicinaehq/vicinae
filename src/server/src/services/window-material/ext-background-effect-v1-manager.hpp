#pragma once
#include <memory>
#include <qobject.h>
#include <qwindow.h>
#include <unordered_map>
#include "ext-background-effect-v1-client-protocol.h"
#include "services/window-material/window-material-backend.hpp"

class ExtBackgroundEffectV1Manager : public WindowMaterialBackend {
public:
  explicit ExtBackgroundEffectV1Manager(ext_background_effect_manager_v1 *manager);

  bool isSupported() const override;
  bool apply(QWindow *win, const Params &params) override;
  bool clear(QWindow *win) override;

protected:
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  struct BlurState {
    ext_background_effect_surface_v1 *effect;
    Params cfg;

    ~BlurState() {
      if (effect) ext_background_effect_surface_v1_destroy(effect);
    }
  };

  void applyBlur(QWindow *win, const BlurState &state);

  ext_background_effect_manager_v1 *m_manager = nullptr;
  std::unordered_map<QWindow *, std::unique_ptr<BlurState>> m_state;
};
