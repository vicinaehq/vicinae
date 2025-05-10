#pragma once
#include "omni-icon.hpp"
#include "ui/button.hpp"
#include "ui/image/omnimg.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>

class IconButton : public Button {
  Omnimg::ImageWidget *_icon;

protected:
  void resizeEvent(QResizeEvent *event) override;

public:
  IconButton();

  void setUrl(const OmniIconUrl &url);
};
