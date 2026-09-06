#pragma once
#include <QCoreApplication>
#include "fuzzy-section.hpp"
#include "services/note/note-db.hpp"

class ManageNotesSection : public FuzzySection<note::Note> {
  Q_DECLARE_TR_FUNCTIONS(ManageNotesSection)

public:
  QString sectionName() const override { return tr("Notes ({count})"); }

  void setOnNoteSelected(std::function<void(const note::Note &)> cb) { m_onNoteSelected = std::move(cb); }

  void onSelected(int i) override {
    if (m_onNoteSelected) m_onNoteSelected(at(i));
  }

protected:
  QString displayTitle(const note::Note &item) const override;
  std::optional<ImageURL> displayIcon(const note::Note &item) const override;
  AccessoryList displayAccessories(const note::Note &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const note::Note &item) const override;

private:
  std::function<void(const note::Note &)> m_onNoteSelected;
};
