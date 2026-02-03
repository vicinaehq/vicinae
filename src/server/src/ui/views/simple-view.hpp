#pragma once
#include "ui/views/base-view.hpp"

class SimpleView : public BaseView {
protected:
  virtual QWidget *centerWidget() const;

  void setupUI(QWidget *centerWidget);

public:
  SimpleView(QWidget *parent = nullptr);
};
