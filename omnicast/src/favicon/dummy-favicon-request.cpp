#include "favicon/dummy-favicon-request.hpp"

void DummyFaviconRequest::start() {
  qDebug() << "Started DummyFaviconRequest!";
  emit failed();
}

DummyFaviconRequest::DummyFaviconRequest(const QString &domain, QObject *parent)
    : AbstractFaviconRequest(domain, parent) {}
