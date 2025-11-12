#pragma once
#include "extend/grid-model.hpp"
#include "ui/omni-list/omni-list.hpp"

class ExtensionGridList : public QWidget {
  Q_OBJECT

signals:
  void selectionChanged(const GridItemViewModel *iten) const;
  void itemActivated(const GridItemViewModel &item) const;

public:
  ExtensionGridList();

  void setColumns(int cols);
  void setAspectRatio(double ratio);
  void setFit(ObjectFit fit);
  void setInset(GridItemContentWidget::Inset inset);

  bool selectUp();
  bool selectDown();
  bool selectLeft();
  bool selectRight();
  bool selectHome();
  bool selectEnd();
  void selectNext();
  void activateCurrentSelection() const;
  GridItemViewModel const *selected() const;
  bool empty() const;
  void setModel(const std::vector<GridChild> &model,
                OmniList::SelectionPolicy selection = OmniList::SelectFirst);
  void setFilter(const QString &query);

private:
  struct SectionGroup {
    const GridSectionModel *section = nullptr;
    std::vector<std::pair<const GridItemViewModel *, int>> scoredItems;
    int bestScore = 0;
  };

  static int computeItemScore(const GridItemViewModel &model, const std::string &query);

  void render(OmniList::SelectionPolicy selectionPolicy);
  void handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                              const OmniList::AbstractVirtualItem *previous);
  void handleItemActivated(const OmniList::AbstractVirtualItem &item);

  OmniList *m_list = new OmniList;
  std::vector<GridChild> m_model;
  int m_columns = 1;
  double m_aspectRatio = 1;
  ObjectFit m_fit = ObjectFit::Contain;
  GridItemContentWidget::Inset m_inset = GridItemContentWidget::Inset::None;
  QString m_filter;
};
