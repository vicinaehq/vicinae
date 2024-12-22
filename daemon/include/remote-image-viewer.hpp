#pragma once
#include <QNetworkAccessManager>
#include <qlabel.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsize.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class RemoteImageViewer : public QWidget {
  Q_OBJECT

  QNetworkAccessManager *net;
  QLabel *label;
  Qt::Alignment align;
  QSize scaled;

private slots:
  void requestFinished(QNetworkReply *);

public:
  RemoteImageViewer();
  ~RemoteImageViewer();

  void load(const QString &url, Qt::Alignment = Qt::AlignCenter,
            QSize size = {});

signals:
  void imageLoaded();
};
