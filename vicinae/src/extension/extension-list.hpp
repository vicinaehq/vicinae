#pragma once
#include "extend/list-model.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/vlist/vlist.hpp"
#include "extension-list-model.hpp"
#include <qobject.h>

class ExtensionList : public QWidget {
  Q_OBJECT

signals:
  void selectionChanged(const ListItemViewModel *iten) const;
  void itemActivated(const ListItemViewModel &item) const;

public:
  ExtensionList();

  bool selectUp();
  bool selectDown();
  void selectNext();
  void activateCurrentSelection() const;
  ListItemViewModel const *selected() const;
  bool empty() const;
  void setModel(const std::vector<ListChild> &model,
                OmniList::SelectionPolicy selection = OmniList::SelectFirst);
  void setFilter(const QString &query);

private:
  void handleSelectionChanged(const std::optional<vicinae::ui::VListModel::Index> idx);
  void handleItemActivated(vicinae::ui::VListModel::Index idx);

  ExtensionListModel *m_model = new ExtensionListModel(this);
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
