#include "remote-image-viewer.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsize.h>

RemoteImageViewer::RemoteImageViewer()
    : net(new QNetworkAccessManager()), label(new QLabel),
      align(Qt::AlignCenter), scaled() {
  auto layout = new QVBoxLayout();

  layout->addWidget(label);
  layout->setContentsMargins(0, 0, 0, 0);

  connect(net, &QNetworkAccessManager::finished, this,
          &RemoteImageViewer::requestFinished);

  setLayout(layout);
}

void RemoteImageViewer::load(const QString &url, Qt::Alignment align,
                             QSize scaled) {
  qDebug() << "fetching " << url;
  this->align = align;
  QNetworkRequest req;

  if (scaled.isValid()) {
    label->setFixedSize(scaled);
  }

  this->scaled = scaled;
  req.setUrl(url);
  net->get(req);
}

void RemoteImageViewer::requestFinished(QNetworkReply *reply) {
  auto buf = reply->readAll();
  QPixmap pix;

  pix.loadFromData(buf);

  int width = label->width();
  int height = label->height();

  if (scaled.isValid()) {
    width = scaled.width();
    height = scaled.height();
  }

  auto scaledPix =
      pix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  label->setAlignment(Qt::AlignCenter);
  label->setPixmap(scaledPix);

  emit imageLoaded();
}

RemoteImageViewer::~RemoteImageViewer() { delete net; }
