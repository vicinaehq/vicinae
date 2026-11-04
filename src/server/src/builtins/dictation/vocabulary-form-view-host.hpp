#pragma once
#include <optional>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "ui/views/bridge-view.hpp"

class VocabularyFormViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(VocabularyFormViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString word READ word WRITE setWord NOTIFY formChanged)
  Q_PROPERTY(QString wordError READ wordError NOTIFY errorsChanged)
  Q_PROPERTY(QString hint READ hint WRITE setHint NOTIFY formChanged)
  Q_PROPERTY(QString hintError READ hintError NOTIFY errorsChanged)

signals:
  void formChanged();
  void errorsChanged();

public:
  VocabularyFormViewHost() = default;
  // Edits the existing entry.
  VocabularyFormViewHost(const QString &word, const QString &hint)
      : m_original(word), m_word(word), m_hint(hint) {}

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString word() const { return m_word; }
  QString wordError() const { return m_wordError; }
  QString hint() const { return m_hint; }
  QString hintError() const { return m_hintError; }

  void setWord(const QString &v) {
    if (m_word != v) {
      m_word = v;
      emit formChanged();
    }
  }

  void setHint(const QString &v) {
    if (m_hint != v) {
      m_hint = v;
      emit formChanged();
    }
  }

private:
  std::optional<QString> m_original;
  QString m_word;
  QString m_wordError;
  QString m_hint;
  QString m_hintError;
};
