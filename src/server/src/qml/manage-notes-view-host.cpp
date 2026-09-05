#include <QDateTime>
#include "manage-notes-view-host.hpp"
#include "note-form-view-host.hpp"
#include "service-registry.hpp"
#include "services/note/note-service.hpp"

QUrl ManageNotesViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap ManageNotesViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ManageNotesViewHost::initialize() {
  BaseView::initialize();
  initModel();

  m_noteService = context()->services->noteService();

  m_section.setOnNoteSelected([this](const note::Note &note) { loadDetail(note); });
  model()->addSource(&m_section);

  setSearchPlaceholderText(tr("Search for notes..."));

  connect(m_noteService, &NoteService::notesChanged, this, &ManageNotesViewHost::reload);

  connect(model(), &QAbstractItemModel::modelReset, this, [this]() {
    if (model()->rowCount() == 0) clearDetail();
  });
}

void ManageNotesViewHost::loadInitialData() { reload(); }

void ManageNotesViewHost::beforePop() {
  clearDetail();
  ListViewHost::beforePop();
}

void ManageNotesViewHost::loadDetail(const note::Note &note) {
  QVariantList meta;

  const auto addRow = [&meta](const QString &label, const QString &value) {
    meta.append(QVariantMap{
        {QStringLiteral("label"), label},
        {QStringLiteral("value"), value},
    });
  };

  addRow(tr("Created at"), QDateTime::fromSecsSinceEpoch(note.createdAt).toString());
  addRow(tr("Updated at"), QDateTime::fromSecsSinceEpoch(note.updatedAt).toString());

  if (note.lastUsedAt) {
    addRow(tr("Last used"), QDateTime::fromSecsSinceEpoch(*note.lastUsedAt).toString());
  }

  if (note.pinnedAt) { addRow(tr("Pinned at"), QDateTime::fromSecsSinceEpoch(*note.pinnedAt).toString()); }

  m_detailMetadata = meta;
  m_detailMarkdown = QString::fromStdString(note.body);
  m_hasDetail = true;

  emit detailChanged();
}

void ManageNotesViewHost::clearDetail() {
  if (!m_hasDetail) return;

  m_hasDetail = false;
  m_detailMarkdown.clear();
  m_detailMetadata.clear();

  emit detailChanged();
}

std::unique_ptr<ActionPanelState> ManageNotesViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  auto *create = new StaticAction(tr("Create note"), BuiltinIcon::Plus, [this](ApplicationContext *ctx) {
    ctx->navigation->pushView(new NoteFormViewHost());
  });

  create->setPrimary(true);
  section->addAction(create);

  return panel;
}

void ManageNotesViewHost::reload() { m_section.setItems(m_noteService->database()->notes()); }
