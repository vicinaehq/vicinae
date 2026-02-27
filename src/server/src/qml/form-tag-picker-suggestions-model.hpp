#pragma once
#include "extend/tag-model.hpp"
#include "fuzzy-list-model.hpp"
#include <QStringList>
#include <memory>

class FormTagPickerSuggestionsModel : public FuzzyListModel<TagPickerItemModel> {
  Q_OBJECT

public:
  enum Role { ValueRole = CommandListModel::Accessory + 1 };

  using FuzzyListModel::FuzzyListModel;

  void setSourceItems(std::vector<TagPickerItemModel> items);
  void setPickedItems(const QStringList &pickedItems);

  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE QString itemValueAt(int row) const;

protected:
  void applyFilter() override;
  QString displayTitle(const TagPickerItemModel &item) const override;
  QString displayIconSource(const TagPickerItemModel &item) const override;
  QString itemId(int section, int item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const TagPickerItemModel &item) const override;

private:
  QStringList m_pickedItems;
};
