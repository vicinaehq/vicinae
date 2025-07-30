#pragma once
#include "simple-view.hpp"

class FormView : public SimpleView {
public:
  FormView(QWidget *parent = nullptr) : SimpleView(parent) {}

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};
