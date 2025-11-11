#pragma once
#include "extend/list-model.hpp"
#include "ui/omni-list/omni-list.hpp"

class ExtensionList : public QWidget {
  Q_OBJECT

signals:
  void selectionChanged(const ListItemViewModel *iten) const;
  void itemActivated(const ListItemViewModel &item) const;

public:
  ExtensionList();

  bool selectUp();
  bool selectDown();
  bool selectHome();
  bool selectEnd();
  void selectNext();
  void activateCurrentSelection() const;
  ListItemViewModel const *selected() const;
  bool empty() const;
  void setModel(const std::vector<ListChild> &model,
                OmniList::SelectionPolicy selection = OmniList::SelectFirst);
  void setFilter(const QString &query);

private:
  struct SectionGroup {
    const ListSectionModel *section = nullptr;
    std::vector<std::pair<const ListItemViewModel *, int>> scoredItems;
    int bestScore = 0; // we could use this to reorder sections
  };

  static int computeItemScore(const ListItemViewModel &model, const std::string &query);

  void render(OmniList::SelectionPolicy selectionPolicy);
  void handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                              const OmniList::AbstractVirtualItem *previous);
  void handleItemActivated(const OmniList::AbstractVirtualItem &item);

  OmniList *m_list = new OmniList;
  std::vector<ListChild> m_model;
  QString m_filter;
};
