#pragma once
#include "image-fetcher.hpp"

class FaviconFetcher {
public:
  static ImageReply *fetch(const QString &domain, QSize size) {
    auto serializedSize = QString("%1x%2").arg(size.width()).arg(size.height());
    auto url = QString("https://icons.duckduckgo.com/ip3/%1.ico").arg(domain);

    qDebug() << "fetch" << url;

    return ImageFetcher::instance().fetch(url);
  }
};
