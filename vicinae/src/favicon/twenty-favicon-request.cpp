#include "favicon/twenty-favicon-request.hpp"
#include <qpixmap.h>
#include <qstringview.h>

void TwentyFaviconRequester::loadingFailed() {
  _currentReply->deleteLater();
  _currentReply = nullptr;

  currentSizeAttemptIndex += 1;

  if (currentSizeAttemptIndex >= sizes.size()) {
    emit failed();
    return;
  }

  tryForCurrentSize();
}

void TwentyFaviconRequester::imageLoaded(const QByteArray &data) {
  QPixmap pixmap;

  pixmap.loadFromData(data);
  _currentReply->deleteLater();
  _currentReply = nullptr;
  emit finished(pixmap);
}

QString TwentyFaviconRequester::makeUrl(uint size) const { return placeholderUrl.arg(domain()).arg(size); }

void TwentyFaviconRequester::tryForCurrentSize() {
  if (currentSizeAttemptIndex >= sizes.size()) { return; }

  auto serviceUrl = makeUrl(sizes.at(currentSizeAttemptIndex));
  auto reply = NetworkFetcher::instance()->fetch(serviceUrl);

  connect(reply, &FetchReply::finished, this, &TwentyFaviconRequester::imageLoaded);
  // connect(reply, &ImageReply::loadingError, this, &TwentyFaviconRequester::loadingFailed);
  _currentReply = reply;
}

void TwentyFaviconRequester::start() {
  currentSizeAttemptIndex = 0;

  if (_currentReply) {
    _currentReply->deleteLater();
    _currentReply = nullptr;
  }

  tryForCurrentSize();
}

TwentyFaviconRequester::TwentyFaviconRequester(const QString &domain, QObject *parent)
    : AbstractFaviconRequest(domain, parent), placeholderUrl("https://twenty-icons.com/%1/%2"),
      _currentReply(nullptr) {}

TwentyFaviconRequester::~TwentyFaviconRequester() {
  if (_currentReply) { _currentReply->deleteLater(); }
}
