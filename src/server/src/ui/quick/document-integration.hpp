#pragma once

#include <document/document-style.hpp>
#include "ui/qml-engine-scope.hpp"

class DocumentIntegration : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  Q_PROPERTY(vicinae::document::DocumentStyle *style READ style CONSTANT)

  Q_INVOKABLE void openLink(const QString &url);
  Q_INVOKABLE QString imageSource(const QString &url) const;

public:
  explicit DocumentIntegration(QObject *parent);
  static DocumentIntegration *create(QQmlEngine *, QJSEngine *) {
    return QmlEngineScope::global<DocumentIntegration>();
  }
  vicinae::document::DocumentStyle *style() { return &m_style; }

private:
  void updateStyle();
  vicinae::document::DocumentStyle m_style{this};
};
