#pragma once
#include <memory>
#include <qobject.h>
#include <qwindow.h>
#include <unordered_map>
#include "ext-background-effect-v1-client-protocol.h"
#include "services/background-effect/abstract-background-effect-manager.hpp"

class ExtBackgroundEffectV1Manager : public AbstractBackgroundEffectManager {
public:
  explicit ExtBackgroundEffectV1Manager(ext_background_effect_manager_v1 *manager);

  bool supportsBlur() const override;
  bool setBlur(QWindow *win, const BlurConfig &cfg) override;
  bool removeBlur(QWindow *win) override;

protected:
  bool eventFilter(QObject *sender, QEvent *event) override;

private:
  struct BlurState {
    ext_background_effect_surface_v1 *effect;
    BlurConfig cfg;

    ~BlurState() {
      if (effect) ext_background_effect_surface_v1_destroy(effect);
    }
  };

  static void capabilities(void *data, ext_background_effect_manager_v1 *, uint32_t flags);

  static constexpr const ext_background_effect_manager_v1_listener s_listener = {
      .capabilities = &ExtBackgroundEffectV1Manager::capabilities};

  void applyBlur(QWindow *win, const BlurState &state);

  ext_background_effect_manager_v1 *m_manager = nullptr;
  std::unordered_map<QWindow *, std::unique_ptr<BlurState>> m_state;
  bool m_supportsBlur = false;
};
