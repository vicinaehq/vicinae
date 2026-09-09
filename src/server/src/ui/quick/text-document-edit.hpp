#pragma once
#include <QObject>
#include <QQuickTextDocument>
#include <QtQml/qqmlregistration.h>

class TextDocumentEdit : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit TextDocumentEdit(QObject *parent = nullptr);

  Q_INVOKABLE void replace(QQuickTextDocument *document, int start, int end, const QString &text);
};
