#pragma once
#include "extend/image-model.hpp"
#include "remote-image-viewer.hpp"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscreen.h>
#include <qtextformat.h>
#include <qwidget.h>
#include <variant>

static QSize defaultIconSize = {32, 32};

class ImageViewer : public QWidget {
public:
  static QWidget *createFromModel(const ImageLikeModel &imageLike, QSize size = {}) {
    if (auto model = std::get_if<ImageUrlModel>(&imageLike)) {
      auto widget = new RemoteImageViewer(model->url, Qt::AlignCenter, size);

      return widget;
    }

    if (auto model = std::get_if<ThemeIconModel>(&imageLike)) {
      auto icon = QIcon::fromTheme(model->iconName);
      auto label = new QLabel();
      QSize iconSize = size.isValid() ? size : defaultIconSize;

      if (icon.isNull()) { icon = QIcon::fromTheme("application-x-executable"); }

      QScreen *screen = QApplication::primaryScreen();
      qreal devicePixelRatio = screen->devicePixelRatio();
      QSize logicalSize = size.isValid() ? size : defaultIconSize;
      QSize scaledSize = logicalSize * devicePixelRatio;

      QPixmap pixmap = icon.pixmap(scaledSize, QIcon::Mode::Normal, QIcon::State::Off);

      /*
  pixmap = pixmap.scaled(size.isValid() ? size : defaultIconSize, Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
                                                     */

      label->setPixmap(pixmap);
      label->setAlignment(Qt::AlignCenter);

      return label;
    }

    return new QWidget();
  }
};
