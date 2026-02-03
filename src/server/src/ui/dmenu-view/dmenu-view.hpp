#pragma once
#include "ui/dmenu-view/dmenu-model.hpp"
#include "ui/views/typed-list-view.hpp"
#include "vicinae-ipc/ipc.hpp"

namespace DMenu {
class View : public TypedListView<DMenuModel> {
  Q_OBJECT

signals:
  void selected(const QString &text) const;
  void aborted() const;

public:
  View(ipc::DMenu::Request data);

protected:
  void hideEvent(QHideEvent *event) override;
  QWidget *generateDetail(const ItemType &item) const override;

private:
  void setFilter(std::string_view query);
  void updateSectionName(std::string_view name);
  std::string expandSection(std::string_view name, size_t count);
  void itemSelected(const ItemType &item) override;

  void emptied() override;
  bool showBackButton() const override { return false; }
  bool onBackspace() override { return true; }
  QString initialNavigationTitle() const override;
  QString initialSearchPlaceholderText() const override;
  void textChanged(const QString &text) override;
  void beforePop() override;
  void selectEntry(const QString &text);
  void initialize() override;

  std::vector<std::string_view> m_entries;
  std::vector<Scored<std::string_view>> m_filteredEntries;
  std::string_view m_sectionNameTemplate;
  std::string m_sectionName;
  bool m_selected = false;
  ipc::DMenu::Request m_data;

  DMenuModel *m_model = nullptr;
};

}; // namespace DMenu
