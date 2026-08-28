#include <QUrl>
#include "note-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/note/note-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"

NoteFormViewHost::NoteFormViewHost(note::Note note, Mode mode)
    : m_mode(mode), m_initialNote(std::move(note)) {}

QUrl NoteFormViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/NoteFormView.qml"));
}

QVariantMap NoteFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void NoteFormViewHost::initialize() {
  BaseView::initialize();

  m_service = context()->services->noteService();

  auto panel = std::make_unique<FormActionPanelState>();
  auto *section = panel->createSection();
  auto *submitAction =
      new StaticAction(tr("Submit"), ImageURL::builtin(BuiltinIcon::EnterKey), [this]() { submit(); });

  section->addAction(submitAction);
  setActions(std::move(panel));

  if (m_initialNote) {
    m_title = QString::fromStdString(m_initialNote->title);
    m_body = QString::fromStdString(m_initialNote->body);
    emit formChanged();

    if (m_mode == Mode::Edit) { setNavigationTitle(tr("Edit \"%1\"").arg(m_title)); }
  }
}

void NoteFormViewHost::submit() {
  const auto toast = context()->services->toastService();

  const note::NotePayload payload{
      .title = m_title.trimmed().toStdString(),
      .body = m_body.toStdString(),
  };

  m_titleError.clear();

  if (payload.title.empty()) {
    m_titleError = tr("Title should not be empty");
    emit errorsChanged();
    toast->failure(tr("Validation failed"));
    return;
  }

  emit errorsChanged();

  if (m_mode == Mode::Edit && m_initialNote) {
    if (const auto result = m_service->updateNote(m_initialNote->id, payload); !result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success(tr("Note updated"));
  } else {
    if (const auto result = m_service->createNote(payload); !result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success(tr("Note created"));
  }

  popSelf();
}
