#pragma once
#include <QVariantList>
#include "builtin_icon.hpp"
#include "list-view-host.hpp"
#include "manage-notes-model.hpp"

class NoteService;

class ManageNotesViewHost : public ListViewHost {
  Q_OBJECT

  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailMarkdown READ detailMarkdown NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)
  Q_PROPERTY(QString emptyTitle MEMBER m_emptyTitle CONSTANT)
  Q_PROPERTY(QString emptyDescription MEMBER m_emptyDescription CONSTANT)
  Q_PROPERTY(ImageUrl emptyIcon MEMBER m_emptyIcon CONSTANT)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const { return m_hasDetail; }
  QString detailMarkdown() const { return m_detailMarkdown; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  void loadDetail(const note::Note &note);
  void clearDetail();
  void reload();

  ManageNotesSection m_section;
  NoteService *m_noteService = nullptr;

  bool m_hasDetail = false;
  QString m_detailMarkdown;
  QVariantList m_detailMetadata;
  QString m_emptyTitle = tr("No notes");
  QString m_emptyDescription = tr("Create a note to get started");
  ImageUrl m_emptyIcon = ImageUrl(ImageURL(BuiltinIcon::BlankDocument));
};
