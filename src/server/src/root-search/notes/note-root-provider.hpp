#pragma once
#include <QCoreApplication>
#include "common.hpp"
#include "services/note/note-db.hpp"
#include "services/note/note-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class RootNoteItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(RootNoteItem)

  note::Note m_note;

  QString title() const override;
  double baseScoreWeight() const override;
  AccessoryList accessories() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  QString typeDisplayName() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;

public:
  explicit RootNoteItem(note::Note note);
};

class NoteRootProvider : public RootProvider {
  NoteService &m_service;

public:
  QString displayName() const override;
  QString uniqueId() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  explicit NoteRootProvider(NoteService &service);
};
