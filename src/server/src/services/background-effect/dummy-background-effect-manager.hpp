#pragma once
#include "abstract-background-effect-manager.hpp"

class DummyBackgroundEffectManager : public AbstractBackgroundEffectManager {
public:
  bool supportsBlur() const override { return false; }
  bool setBlur(QWindow *win, const BlurConfig &cfg) override { return false; }
  bool removeBlur(QWindow *win) override { return false; }
};
