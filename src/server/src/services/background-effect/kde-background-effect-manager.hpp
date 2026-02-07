#pragma once
#include <memory>
#include <qwindow.h>
#include <unordered_map>
#include "kde-blur-client-protocol.h"
#include "services/background-effect/abstract-background-effect-manager.hpp"

namespace KDE {
class BackgroundEffectManager : public AbstractBackgroundEffectManager {
public:
  explicit BackgroundEffectManager(org_kde_kwin_blur_manager *manager);

  bool supportsBlur() const override;
  bool setBlur(QWindow *win, const BlurConfig &cfg) override;
  bool removeBlur(QWindow *win) override;

private:
  struct BlurState {
    org_kde_kwin_blur *blur;
    BlurConfig cfg;

    ~BlurState() { org_kde_kwin_blur_release(blur); }
  };

  bool eventFilter(QObject *sender, QEvent *event) override;

  void roundtrip();
  void applyBlur(QWindow *win, const BlurState &state);

  std::unordered_map<QWindow *, std::unique_ptr<BlurState>> m_state;
  org_kde_kwin_blur_manager *m_manager = nullptr;
};
} // namespace KDE
