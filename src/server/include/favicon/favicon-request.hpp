#include <qobject.h>
#pragma once

class AbstractFaviconRequest : public QObject {
  Q_OBJECT
  QString _domain;

public:
  QString domain() const;
  AbstractFaviconRequest(const QString &domain, QObject *parent = nullptr);
  virtual void start() = 0;

signals:
  void finished(QPixmap favicon) const;
  void failed() const;
};
