#pragma once
#include "services/background-effect/abstract-background-effect-manager.hpp"

class DummyBackgroundEffectManager : public AbstractBackgroundEffectManager {
public:
  bool setBlur(QWindow *win, const BlurConfig &cfg) override { return false; }
  bool removeBlur(QWindow *win) override { return false; }
};
