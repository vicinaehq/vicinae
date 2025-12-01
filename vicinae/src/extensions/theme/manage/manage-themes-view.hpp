#pragma once
#include "ui/views/typed-list-view.hpp"
#include "theme-list-model.hpp"
#include "theme-list-controller.hpp"

class ManageThemesView : public TypedListView<ThemeListModel> {
public:
  ManageThemesView();

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;

private:
  void initialize() override;
  void beforePop() override;
  void textChanged(const QString &s) override;
  void itemSelected(const ItemType &item) override;

  ThemeListModel *m_model = nullptr;
  ThemeListController *m_controller = nullptr;
};
