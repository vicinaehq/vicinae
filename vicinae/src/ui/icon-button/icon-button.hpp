#pragma once
#include "../image/url.hpp"
#include "ui/button-base/button-base.hpp"
#include "ui/image/image.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>

class IconButton : public ButtonBase {
public:
  IconButton();

  void setUrl(const ImageURL &url);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  ImageWidget *m_icon;
};
