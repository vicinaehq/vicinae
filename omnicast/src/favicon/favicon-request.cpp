#include "favicon/favicon-request.hpp"

QString AbstractFaviconRequest::domain() const { return _domain; }

AbstractFaviconRequest::AbstractFaviconRequest(const QString &domain, QObject *parent)
    : QObject(parent), _domain(domain) {}
