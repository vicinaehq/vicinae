#pragma once
#include "layout.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/thumbnail/thumbnail.hpp"
#include <qscrollarea.h>
#include <qtmetamacros.h>

class ScreenshotList : public QScrollArea {
  Q_OBJECT

signals:
  void clickedUrl(const ImageURL &url);

public:
  void setUrls(const std::vector<QUrl> &urls) {
    setWidget(createWidget(urls));
    setWidgetResizable(true);
    setAutoFillBackground(true);
  }

  QWidget *createWidget(const std::vector<QUrl> &urls) {
    auto makeThumbnail = [this](const QUrl &url) -> Thumbnail * {
      auto thumbnail = new Thumbnail;
      double aspectRatio = 16 / 10.f;

      connect(thumbnail, &Thumbnail::clicked, this, [this, url]() { emit clickedUrl(ImageURL::http(url)); });

      thumbnail->setClickable(true);
      thumbnail->setFixedHeight(150);
      thumbnail->setFixedWidth(150 * aspectRatio);
      thumbnail->setImage(ImageURL::http(url));

      return thumbnail;
    };

    return HStack().map(urls, makeThumbnail).addStretch().spacing(10).buildWidget();
  }

  void setupUI() {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBar(new OmniScrollBar);
  }

  ScreenshotList() { setupUI(); }
};
