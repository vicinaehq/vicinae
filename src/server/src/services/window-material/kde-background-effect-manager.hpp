#pragma once
#include <memory>
#include <qwindow.h>
#include <unordered_map>
#include "kde-blur-client-protocol.h"
#include "services/window-material/window-material-backend.hpp"

namespace KDE {
class BackgroundEffectManager : public WindowMaterialBackend {
public:
  explicit BackgroundEffectManager(org_kde_kwin_blur_manager *manager);

  bool isSupported() const override;
  bool apply(QWindow *win, const Params &params) override;
  bool clear(QWindow *win) override;

private:
  struct BlurState {
    org_kde_kwin_blur *blur;
    Params cfg;

    ~BlurState() { org_kde_kwin_blur_release(blur); }
  };

  bool eventFilter(QObject *sender, QEvent *event) override;

  void roundtrip();
  void applyBlur(QWindow *win, const BlurState &state);

  std::unordered_map<QWindow *, std::unique_ptr<BlurState>> m_state;
  org_kde_kwin_blur_manager *m_manager = nullptr;
};
} // namespace KDE
