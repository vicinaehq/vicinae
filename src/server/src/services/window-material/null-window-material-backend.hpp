#pragma once
#include "services/window-material/window-material-backend.hpp"

class NullWindowMaterialBackend : public WindowMaterialBackend {
public:
  bool isSupported() const override { return false; }
  bool apply(QWindow *win, const Params &params) override { return false; }
  bool clear(QWindow *win) override { return false; }
};
