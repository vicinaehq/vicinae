#pragma once
#include <QVariantList>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/views/detail-list-view-host.hpp"
#include "builtins/note/manage-notes-model.hpp"

class NoteService;

class ManageNotesViewHost : public DetailListViewHost {
  Q_OBJECT

public:
  ManageNotesViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const override { return m_hasDetail; }
  QString detailContent() const override { return {}; }
  QString detailMarkdown() const override { return m_detailMarkdown; }
  QVariantList detailMetadata() const override { return m_detailMetadata; }

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
};
