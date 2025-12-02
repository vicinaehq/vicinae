#pragma once
#include "extend/grid-model.hpp"
#include "extension/extension-grid-model.hpp"
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
  void selectNext();
  void activateCurrentSelection() const;
  GridItemViewModel const *selected() const;
  bool empty() const;
  void setModel(const std::vector<GridChild> &model,
                OmniList::SelectionPolicy selection = OmniList::SelectFirst);
  void setFilter(const QString &query);

private:
  void handleSelectionChanged(const std::optional<vicinae::ui::VListModel::Index> idx);

  void handleItemActivated(vicinae::ui::VListModel::Index idx);

  int m_columns = 1;
  double m_aspectRatio = 1;
  ObjectFit m_fit = ObjectFit::Contain;
  GridItemContentWidget::Inset m_inset = GridItemContentWidget::Inset::None;
  QString m_filter;

  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
  ExtensionGridModel *m_model = new ExtensionGridModel(this);
};
