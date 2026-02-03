#include "favicon/google-favicon-request.hpp"
#include <qpixmap.h>
#include <qstringview.h>

void GoogleFaviconRequester::loadingFailed() {
  _currentReply->deleteLater();
  _currentReply = nullptr;

  currentSizeAttemptIndex += 1;

  if (currentSizeAttemptIndex >= sizes.size()) {
    emit failed();
    return;
  }

  tryForCurrentSize();
}

void GoogleFaviconRequester::imageLoaded(const QByteArray &data) {
  QPixmap pixmap;

  pixmap.loadFromData(data);
  _currentReply->deleteLater();
  _currentReply = nullptr;
  emit finished(pixmap);
}

QString GoogleFaviconRequester::makeUrl(uint size) const { return placeholderUrl.arg(domain()).arg(size); }

void GoogleFaviconRequester::tryForCurrentSize() {
  if (currentSizeAttemptIndex >= sizes.size()) { return; }

  auto serviceUrl = makeUrl(sizes.at(currentSizeAttemptIndex));
  auto reply = NetworkFetcher::instance()->fetch(serviceUrl);

  connect(reply, &FetchReply::finished, this, &GoogleFaviconRequester::imageLoaded);
  // connect(reply, &ImageReply::loadingError, this, &GoogleFaviconRequester::loadingFailed);
  _currentReply = reply;
}

void GoogleFaviconRequester::start() {
  currentSizeAttemptIndex = 0;

  if (_currentReply) {
    _currentReply->deleteLater();
    _currentReply = nullptr;
  }

  tryForCurrentSize();
}

GoogleFaviconRequester::GoogleFaviconRequester(const QString &domain, QObject *parent)
    : AbstractFaviconRequest(domain, parent),
      placeholderUrl("https://www.google.com/s2/favicons?domain=%1&sz=%2"), _currentReply(nullptr) {}

GoogleFaviconRequester::~GoogleFaviconRequester() {
  if (_currentReply) { _currentReply->deleteLater(); }
}
