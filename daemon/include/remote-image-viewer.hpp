#pragma once
#include <QNetworkAccessManager>
#include <qlabel.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class RemoteImageViewer : public QWidget {
  Q_OBJECT

  QLabel *label;
  QString url;
  Qt::Alignment align;
  QSize scaled;
  int maxCacheSize = 128 * 128;

private slots:
  void loaded(QPixmap pix);

public:
  RemoteImageViewer(const QString &url, Qt::Alignment = Qt::AlignCenter,
                    QSize size = {});
  ~RemoteImageViewer();

signals:
  void imageLoaded();
};
