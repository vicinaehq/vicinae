#pragma once
#include <qobject.h>
#include <QByteArray>

class AbstractFaviconRequest : public QObject {
  Q_OBJECT
  QString _domain;

public:
  QString domain() const;
  AbstractFaviconRequest(const QString &domain, QObject *parent = nullptr);
  virtual void start() = 0;

signals:
  void finished(QByteArray data) const;
  void failed() const;
};
