#pragma once
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "ui/views/bridge-view.hpp"

class VocabularyFormViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(VocabularyFormViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString word READ word WRITE setWord NOTIFY formChanged)
  Q_PROPERTY(QString wordError READ wordError NOTIFY errorsChanged)

signals:
  void formChanged();
  void errorsChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString word() const { return m_word; }
  QString wordError() const { return m_wordError; }

  void setWord(const QString &v) {
    if (m_word != v) {
      m_word = v;
      emit formChanged();
    }
  }

private:
  QString m_word;
  QString m_wordError;
};
