#pragma once
#include <QList>
#include <QString>
#include <QWidget>
#include <cmath>
#include <qjsonobject.h>
#include <qtmetamacros.h>

class ExtensionComponent : public QWidget {
  Q_OBJECT

public:
signals:
  void extensionEvent(const QString &action, const QJsonObject &payload);
};
