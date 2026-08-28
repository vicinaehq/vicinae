#pragma once
#include <optional>
#include "bridge-view.hpp"
#include "services/note/note-db.hpp"

class NoteService;

class NoteFormViewHost : public FormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY formChanged)
  Q_PROPERTY(QString body READ body WRITE setBody NOTIFY formChanged)
  Q_PROPERTY(QString titleError READ titleError NOTIFY errorsChanged)

signals:
  void formChanged();
  void errorsChanged();

public:
  enum class Mode { Create, Edit };

  NoteFormViewHost() = default;
  NoteFormViewHost(note::Note note, Mode mode);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString title() const { return m_title; }
  QString body() const { return m_body; }
  QString titleError() const { return m_titleError; }

  void setTitle(const QString &v) {
    if (m_title != v) {
      m_title = v;
      emit formChanged();
    }
  }

  void setBody(const QString &v) {
    if (m_body != v) {
      m_body = v;
      emit formChanged();
    }
  }

private:
  Mode m_mode = Mode::Create;
  std::optional<note::Note> m_initialNote;
  NoteService *m_service = nullptr;

  QString m_title;
  QString m_body;
  QString m_titleError;
};
