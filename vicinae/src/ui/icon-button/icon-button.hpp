#pragma once
#include "../image/url.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/button/button.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>

class IconButton : public OmniButtonWidget {
  Omnimg::ImageWidget *_icon;

protected:
  void resizeEvent(QResizeEvent *event) override;

public:
  IconButton();

  void setUrl(const ImageURL &url);
};
