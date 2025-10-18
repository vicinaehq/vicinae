#pragma once
#include "ui/views/list-view.hpp"

class ManageThemesView : public ListView {
public:
  ManageThemesView();

private:
  void generateList(const QString &query);
  void initialize() override;
  void beforePop() override;
  void textChanged(const QString &s) override;
  void itemSelected(const OmniList::AbstractVirtualItem *item) override;
};
