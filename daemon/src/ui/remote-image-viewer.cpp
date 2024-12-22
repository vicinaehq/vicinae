#include "remote-image-viewer.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>

RemoteImageViewer::RemoteImageViewer()
    : net(new QNetworkAccessManager()), label(new QLabel),
      align(Qt::AlignCenter) {
  auto layout = new QVBoxLayout();

  layout->addWidget(label);

  connect(net, &QNetworkAccessManager::finished, this,
          &RemoteImageViewer::requestFinished);

  setLayout(layout);
}

void RemoteImageViewer::load(const QString &url, Qt::Alignment align) {
  this->align = align;
  QNetworkRequest req;

  req.setUrl(url);
  net->get(req);
}

void RemoteImageViewer::requestFinished(QNetworkReply *reply) {
  auto buf = reply->readAll();
  QPixmap pix;

  pix.loadFromData(buf);

  auto scaledPix = pix.scaled(label->width(), label->height(),
                              Qt::KeepAspectRatio, Qt::SmoothTransformation);

  label->setAlignment(Qt::AlignCenter);
  label->setPixmap(scaledPix);

  emit imageLoaded();
}

RemoteImageViewer::~RemoteImageViewer() { delete net; }
