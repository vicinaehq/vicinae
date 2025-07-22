#pragma once
#include "base-view.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-scroll-bar.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qwidget.h>

class StoreDetailHeader : public QWidget {
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget(this);
  TypographyWidget *m_title = new TypographyWidget(this);
  TypographyWidget *m_author = new TypographyWidget(this);
  Omnimg::ImageWidget *m_avatar = new Omnimg::ImageWidget(this);

  QWidget *makeVerticalContainer() {
    auto widget = new QWidget;
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 4, 0, 4);
    layout->addWidget(m_title);
    layout->addStretch();
    layout->addWidget(m_author);
    widget->setLayout(layout);

    return widget;
    // add author + avatar name
  }

  void setupUI() {
    auto layout = new QHBoxLayout;

    m_title->setSize(TextSize::TextTitle);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    layout->addWidget(m_icon);
    layout->setAlignment(Qt::AlignVCenter);
    layout->addWidget(makeVerticalContainer());
    layout->addStretch();

    m_icon->setFixedSize({64, 64});

    setLayout(layout);
  }

public:
  void setTitle(const QString &title) { m_title->setText(title); }
  void setIcon(const OmniIconUrl &url) { m_icon->setUrl(url); }
  void setAuthor(const QString &author, const OmniIconUrl &icon) {
    m_author->setText(author);
    m_avatar->setUrl(icon);
  }
  StoreDetailHeader() { setupUI(); }
};

class ScreenshotList : public QScrollArea {

  QWidget *createWidget(const std::vector<QUrl> &urls) {
    auto widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);

    for (const auto &url : urls) {
      auto image = new Omnimg::ImageWidget;
      double aspectRatio = 16 / 10.f;

      image->setAlignment(Qt::AlignLeft);
      image->setFixedHeight(150);
      image->setFixedWidth(150 * aspectRatio);
      image->setUrl(HttpOmniIconUrl(url));
      layout->addWidget(image);
    }

    layout->addStretch();
    widget->setLayout(layout);

    return widget;
  }

  void setupUI() {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBar(new OmniScrollBar);
  }

public:
  void setUrls(const std::vector<QUrl> &urls) {
    setWidget(createWidget(urls));
    setWidgetResizable(true);
    setAutoFillBackground(true);
  }

  ScreenshotList() { setupUI(); }
};

class RaycastStoreDetailView : public BaseView {
  StoreDetailHeader *m_header = new StoreDetailHeader;
  ScreenshotList *m_screenshotList = new ScreenshotList;

  // section with header and preview images
  QWidget *createPresentationSection() {
    auto container = new QWidget;
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);
    layout->addWidget(m_header);
    layout->addWidget(new HDivider);
    layout->addWidget(m_screenshotList);
    // add image previews
    layout->addStretch();
    container->setLayout(layout);

    return container;
  }

  void setupUI(const Raycast::Extension &extension) {
    auto layout = new QVBoxLayout;

    m_header->setMaximumHeight(70);

    m_screenshotList->setUrls(extension.screenshots());

    m_header->setTitle(extension.title);
    m_header->setIcon(HttpOmniIconUrl(extension.icons.light));
    m_header->setAuthor(extension.author.name, extension.author.avatar);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(createPresentationSection());
    layout->addStretch();
    setLayout(layout);
  }

public:
  RaycastStoreDetailView(const Raycast::Extension &extension) {
    setupUI(extension);

    for (const auto &url : extension.screenshots()) {
      qDebug() << "asset url" << url;
    }
  }
};
