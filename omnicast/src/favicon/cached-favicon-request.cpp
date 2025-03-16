#include "favicon/cached-favicon-request.hpp"

void CachedFaviconRequest::start() { emit finished(_data); }

CachedFaviconRequest::CachedFaviconRequest(const QString &domain, const QPixmap &data, QObject *parent)
    : AbstractFaviconRequest(domain, parent), _data(data) {}
