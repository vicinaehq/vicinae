#pragma once
#include "bridge-view.hpp"
#include "services/snippet/snippet-db.hpp"
#include <optional>

class SnippetService;

class SnippetFormViewHost : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY formChanged)
  Q_PROPERTY(QString content READ content WRITE setContent NOTIFY formChanged)
  Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY formChanged)
  Q_PROPERTY(bool expandAsWord READ expandAsWord WRITE setExpandAsWord NOTIFY formChanged)

  Q_PROPERTY(QString nameError READ nameError NOTIFY errorsChanged)
  Q_PROPERTY(QString contentError READ contentError NOTIFY errorsChanged)
  Q_PROPERTY(QString keywordError READ keywordError NOTIFY errorsChanged)

public:
  enum class Mode { Create, Edit, Duplicate };

  explicit SnippetFormViewHost();
  SnippetFormViewHost(snippet::SerializedSnippet snippet, Mode mode);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString name() const { return m_name; }
  QString content() const { return m_content; }
  QString keyword() const { return m_keyword; }
  bool expandAsWord() const { return m_expandAsWord; }

  QString nameError() const { return m_nameError; }
  QString contentError() const { return m_contentError; }
  QString keywordError() const { return m_keywordError; }

  void setName(const QString &v) {
    if (m_name != v) {
      m_name = v;
      emit formChanged();
    }
  }
  void setContent(const QString &v) {
    if (m_content != v) {
      m_content = v;
      emit formChanged();
    }
  }
  void setKeyword(const QString &v) {
    if (m_keyword != v) {
      m_keyword = v;
      emit formChanged();
    }
  }
  void setExpandAsWord(bool v) {
    if (m_expandAsWord != v) {
      m_expandAsWord = v;
      emit formChanged();
    }
  }

signals:
  void formChanged();
  void errorsChanged();

private:
  Mode m_mode = Mode::Create;
  std::optional<snippet::SerializedSnippet> m_initialSnippet;
  SnippetService *m_service = nullptr;

  QString m_name;
  QString m_content;
  QString m_keyword;
  bool m_expandAsWord = true;

  QString m_nameError;
  QString m_contentError;
  QString m_keywordError;
};
